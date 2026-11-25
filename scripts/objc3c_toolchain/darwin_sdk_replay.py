"""Re-admit retained Apple Silicon SDK evidence and exercise managed installation."""
from __future__ import annotations

import argparse
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import sys
import tarfile
import unicodedata
from zipfile import ZipFile

from . import darwin_evidence, darwin_sdk, darwin_sdk_arc
from .darwin_bootstrap import ROOT
from .darwin_registration import NATIVE, PORTABLE_HEADERS, PORTABLE_SERVICES
from .package import _file_hash, verify_sdk


SOURCE_WORKFLOW = '.github/workflows/darwin-bootstrap.yml'
SCHEMA = 'objc3c.experimental-darwin-sdk-managed-replay.v1'
SHA256 = ('evidence_zip_sha256', 'terminal_zip_sha256', 'terminal_sha256')
NUMBERS = ('run_id', 'run_attempt', 'evidence_artifact_id', 'terminal_artifact_id')
ZIP_LIMIT = 1024**3 + 32 * 1024**2
ARTIFACT_NAMES = {'evidence': 'darwin-arm64-bootstrap', 'terminal': 'darwin-terminal'}


def parse_pin(value: str) -> dict:
    pin = json.loads(value)
    if (not isinstance(pin, dict) or pin.keys() != {*NUMBERS, *SHA256, 'revision'} or
            any(type(pin[name]) is not int or pin[name] <= 0 for name in NUMBERS) or
            pin['evidence_artifact_id'] == pin['terminal_artifact_id'] or
            not isinstance(pin['revision'], str) or not re.fullmatch('[a-f0-9]{40}', pin['revision']) or
            any(not isinstance(pin[name], str) or not darwin_sdk.HEX_SHA.fullmatch(pin[name]) for name in SHA256)):
        raise ValueError('Darwin replay requires exact run, attempt, revision, artifact IDs and SHA-256 pins')
    return pin


def _api(repo: str, route: str):
    return json.loads(subprocess.check_output(['gh', 'api', f'repos/{repo}/{route}']))


def verify_receiver(revision: str) -> None:
    current = subprocess.check_output(['git', '-C', ROOT, 'rev-parse', 'HEAD'], text=True).strip()
    if revision != current:
        raise ValueError('Darwin replay receiver differs from the checked-out workflow revision')


def verify_run(run: dict, repo: str, pin: dict) -> None:
    if (run.get('id') != pin['run_id'] or run.get('run_attempt') != pin['run_attempt'] or
            run.get('head_sha') != pin['revision'] or run.get('path') != SOURCE_WORKFLOW or
            run.get('status') != 'completed' or run.get('conclusion') != 'success' or
            (run.get('head_repository') or {}).get('full_name') != repo or
            (run.get('repository') or {}).get('full_name') != repo):
        raise ValueError('Darwin replay source must be a successful exact attempt from the repository bootstrap workflow')


def artifact_name(pin: dict, role: str) -> str:
    return f"{ARTIFACT_NAMES[role]}-{pin['run_id']}-{pin['run_attempt']}"


def verify_artifact(info: dict, pin: dict, run: dict, *, role: str) -> None:
    workflow = info.get('workflow_run') or {}
    repository = run['repository']['id']
    limit = darwin_evidence.RECEIPT_BYTES if role == 'terminal' else ZIP_LIMIT
    if (info.get('id') != pin[role + '_artifact_id'] or info.get('name') != artifact_name(pin, role) or
            info.get('digest') != 'sha256:' + pin[role + '_zip_sha256'] or info.get('expired') is not False or
            type(info.get('size_in_bytes')) is not int or not 0 < info['size_in_bytes'] <= limit or
            workflow.get('id') != pin['run_id'] or workflow.get('head_sha') != pin['revision'] or
            workflow.get('repository_id') != repository or workflow.get('head_repository_id') != repository):
        raise ValueError(f'Darwin replay {role} artifact does not match its immutable run, attempt and digest pins')


def _source_snapshot(repo: str, revision: str, output: Path) -> None:
    available = subprocess.run(['git', '-C', ROOT, 'cat-file', '-e', revision + '^{commit}'], capture_output=True)
    if available.returncode:
        subprocess.run(['git', '-C', ROOT, 'fetch', '--no-tags', '--depth=1',
                        f'https://github.com/{repo}.git', revision], check=True)
    paths = ['scripts/objc3c_toolchain', 'toolchains/llvm/manifest.json', 'toolchains/llvm/experimental',
             'native/objc3c/src/runtime/platform/darwin_registration.c',
             'tests/clang_foundation/darwin_registration', 'tests/clang_foundation/darwin_portable_services',
             'tests/clang_foundation/darwin_sanitizers']
    paths.extend(str((NATIVE / name).relative_to(ROOT)) for name in PORTABLE_HEADERS)
    for service, fixture in PORTABLE_SERVICES.values():
        paths.extend((f'native/objc3c/src/runtime/stdlib/{service}.cpp', f'native/objc3c/tests/{fixture}.cpp'))
    snapshot = subprocess.check_output(['git', '-C', ROOT, 'archive', revision, '--', *paths])
    output.mkdir()
    with tarfile.open(fileobj=io.BytesIO(snapshot)) as archive:
        archive.extractall(output, filter='data')


def prepare(repo: str, pin: dict, output: Path) -> None:
    if not re.fullmatch(r'[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+', repo):
        raise ValueError('Darwin replay requires an exact GitHub repository name')
    run = _api(repo, f"actions/runs/{pin['run_id']}/attempts/{pin['run_attempt']}")
    verify_run(run, repo, pin)
    jobs = _api(repo, f"actions/runs/{pin['run_id']}/attempts/{pin['run_attempt']}/jobs")['jobs']
    gates = {'Probe Apple Silicon toolchain and Mach-O prerequisites', 'Verify retained Apple Silicon evidence'}
    successful = {job['name'] for job in jobs if job.get('status') == 'completed' and
                  job.get('conclusion') == 'success' and job.get('head_sha') == pin['revision'] and
                  job.get('run_id') == pin['run_id'] and job.get('run_attempt') == pin['run_attempt']}
    if not gates <= successful:
        raise ValueError('Darwin replay source omitted the successful native producer or terminal gate')
    output.mkdir(parents=True)
    for role in ARTIFACT_NAMES:
        artifact_id = pin[role + '_artifact_id']
        verify_artifact(_api(repo, f'actions/artifacts/{artifact_id}'), pin, run, role=role)
    _source_snapshot(repo, pin['revision'], output / 'source')
    (output / 'pin.json').write_text(json.dumps(pin, indent=2) + '\n')


def extract(path: Path, expected_sha256: str, output: Path, *, terminal: bool = False) -> None:
    limit = darwin_evidence.RECEIPT_BYTES if terminal else ZIP_LIMIT
    if path.is_symlink() or not path.is_file() or path.stat().st_size > limit or _file_hash(path) != expected_sha256:
        raise ValueError('Darwin replay ZIP differs from its admitted artifact bytes')
    with ZipFile(path) as archive:
        members = archive.infolist()
        names = {unicodedata.normalize('NFC', member.filename.rstrip('/')).casefold() for member in members}
        if (not 0 < len(members) <= darwin_sdk.MAX_ENTRIES or len(names) != len(members) or
                sum(item.file_size for item in members) > limit or
                any(PurePosixPath(item.filename).is_absolute() or
                    any(part in ('', '.', '..') for part in item.filename.rstrip('/').split('/')) or
                    '\\' in item.filename or stat.S_IFMT(item.external_attr >> 16) not in
                    (0, stat.S_IFREG, stat.S_IFDIR) for item in members) or
                (terminal and archive.namelist() != ['darwin-terminal.json'])):
            raise ValueError('Darwin replay ZIP contains unowned or excessive members')
        output.mkdir()
        archive.extractall(output)


def _admit(directory: Path) -> tuple[Path, dict, dict]:
    pin = parse_pin((directory / 'pin.json').read_text())
    download = directory / 'download'
    for role in ARTIFACT_NAMES:
        path = download / (artifact_name(pin, role) + '.zip')
        extract(path, pin[role + '_zip_sha256'], directory / role, terminal=role == 'terminal')
    terminal = directory / 'terminal/darwin-terminal.json'
    if _file_hash(terminal) != pin['terminal_sha256']:
        raise ValueError('Darwin terminal receipt SHA-256 differs from its pin')
    mode = json.loads(terminal.read_bytes())['mode']
    if mode not in ('package', 'package-sanitizers'):
        raise ValueError('Darwin managed replay requires retained experimental package evidence')
    evidence = (directory / 'evidence').resolve(strict=True)
    clean_env = {'PATH': os.environ['PATH'], 'LC_ALL': 'C'}
    reproduced = json.loads(subprocess.check_output(
        [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain.darwin_evidence',
         '--evidence', evidence, '--revision', pin['revision'], '--outcome', 'success', '--mode', mode],
        cwd=directory / 'source', env=clean_env))
    admitted = darwin_evidence.admit_terminal(reproduced, terminal, pin['terminal_sha256'])
    return evidence / 'runtime/sdk/package' / darwin_sdk.ARCHIVE_NAME, admitted, pin


def qualify(directory: Path, output: Path, *, receiver_revision: str) -> dict:
    host = darwin_sdk._host()
    primary = _admit(directory / 'primary')
    replacement = _admit(directory / 'replacement') if (directory / 'replacement').exists() else None
    if replacement and primary[1]['seal_sha256'] == replacement[1]['seal_sha256']:
        raise ValueError('Darwin managed update requires another independently admitted SDK seal')
    output.mkdir(parents=True)
    root = output / 'Managed Apple Silicon SDK'
    candidates = (primary, replacement) if replacement else (primary,)
    report = {'schema': SCHEMA, 'status': 'FAIL', 'support_truth': False, 'host': host,
              'receiver_revision': receiver_revision,
              'candidates': [{'pin': pin, 'admission': admission} for _, admission, pin in candidates],
              'commands': [], 'operations': {}}

    def managed(*arguments):
        return json.loads(darwin_sdk._run(sys.executable, '-B', '-m', 'scripts.objc3c_toolchain',
            'darwin', *arguments, '--root', root, cwd=ROOT, commands=report['commands']))

    def activate(candidate, phase, expected_action):
        archive, admission, _ = candidate
        row = managed('install', '--archive', archive, '--archive-sha256', admission['archive_sha256'])
        report['operations'][phase] = row
        if row['action'] != expected_action or row['seal_sha256'] != admission['seal_sha256'] or 'cleanup_error' in row:
            raise ValueError(f'Darwin managed replay failed during {phase}')
        prefix = (root / 'current').resolve(strict=True)
        manifest = verify_sdk(prefix, admission['seal_sha256'])
        report['operations'][phase + '_arc'] = darwin_sdk_arc.qualify_consumer(
            prefix, manifest, host, output / (phase + ' ARC consumer'), report['commands'])
        return prefix

    try:
        prefix = activate(primary, 'install', 'installed')
        before = (prefix / 'bin/clang').stat()
        reused = managed('install', '--archive', primary[0], '--archive-sha256', primary[1]['archive_sha256'])
        report['operations']['reuse'] = reused
        if (reused['action'] != 'reused' or reused['seal_sha256'] != primary[1]['seal_sha256'] or
                (root / 'current').resolve(strict=True) != prefix or
                not os.path.samestat(before, (prefix / 'bin/clang').stat())):
            raise ValueError('Darwin managed repeat install changed the active SDK')
        final = primary
        if replacement:
            activate(replacement, 'update', 'updated')
            if prefix.exists():
                raise ValueError('Darwin managed update retained the earlier SDK')
            final = replacement
        seal = final[1]['seal_sha256']
        removed = managed('uninstall', '--seal-sha256', seal)
        report['operations']['uninstall'] = removed
        if (removed['action'] != 'uninstalled' or removed['seal_sha256'] != seal or
                (root / 'current').is_symlink() or list(root.glob('sdk-*'))):
            raise ValueError('Darwin managed replay retained an active or retired SDK')
        report['status'] = 'PASS'
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        report['error'] = str(error)
        raise
    finally:
        (output / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    actions = parser.add_subparsers(dest='action', required=True)
    metadata = actions.add_parser('prepare')
    metadata.add_argument('--repo', required=True)
    metadata.add_argument('--output-dir', type=Path, required=True)
    metadata.add_argument('--receiver-revision', required=True)
    metadata.add_argument('--github-output', type=Path, required=True)
    replay = actions.add_parser('qualify')
    replay.add_argument('--input-dir', type=Path, required=True)
    replay.add_argument('--output-dir', type=Path, required=True)
    replay.add_argument('--receiver-revision', required=True)
    args = parser.parse_args()
    verify_receiver(args.receiver_revision)
    if args.action == 'prepare':
        pins = {'primary': parse_pin(os.environ['DARWIN_REPLAY_CANDIDATE'])}
        if replacement := os.environ.get('DARWIN_REPLAY_REPLACEMENT'):
            pins['replacement'] = parse_pin(replacement)
        with args.github_output.open('a') as outputs:
            for owner, pin in pins.items():
                prepare(args.repo, pin, args.output_dir / owner)
                outputs.write(f"{owner}_run_id={pin['run_id']}\n")
                outputs.write(f"{owner}_artifact_ids={pin['evidence_artifact_id']},{pin['terminal_artifact_id']}\n")
    else:
        result = qualify(args.input_dir.resolve(), args.output_dir.resolve(), receiver_revision=args.receiver_revision)
        print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
