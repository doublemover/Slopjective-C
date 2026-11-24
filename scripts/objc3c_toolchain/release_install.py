"""Execute and record a native clean-container Linux SDK installation."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time

from .install_sdk import file_hash, read_archive_admission
from .linux_host import HOSTS, host_identity
from .package import SCHEMA as SDK_SCHEMA, _digest


DISTRIBUTIONS = ('debian:13', 'ubuntu:26.04')
SCOPES = ('smoke', 'package-build', 'package-surface', 'package-modules',
          'async', 'isolation', 'features', 'carriers')
SCHEMA = 'objc3c.linux-archive-install.v1'
SCOPE_SCHEMA = 'objc3c.linux-archive-install-scope.v1'
IMAGE = re.compile(r'[^\s@]+@sha256:[a-f0-9]{64}')
IMAGE_ID = re.compile(r'sha256:[a-f0-9]{64}')
RELEASE_SCHEMA = 'objc3c.linux-release.v1'
SHA256 = re.compile(r'[a-f0-9]{64}')
INSTALLER_COMPANIONS = ('install-linux.sh', 'install-sdk.py')


def validate_clean_install(row: dict, *, host: str, distribution: str, revision: str,
                           run: int, attempt: int, archive: dict) -> None:
    expected = {'schema': SCHEMA, 'status': 'PASS', 'host': host, 'revision': revision,
                'run': run, 'attempt': attempt, 'distribution': distribution,
                'archive_sha256': archive['sha256'], 'seal_sha256': archive['seal_sha256'],
                'source_build_id': archive['source_build_id']}
    scopes = row.get('scopes')
    if (any(row.get(key) != value for key, value in expected.items()) or
            not IMAGE.fullmatch(row.get('image', '')) or not IMAGE_ID.fullmatch(row.get('image_id', '')) or
            not isinstance(scopes, dict) or set(scopes) != set(SCOPES) or
            not all(isinstance(value, str) and SHA256.fullmatch(value) for value in scopes.values())):
        raise ValueError(f'{host} {distribution} installation receipt differs from the qualified archive')


def published_archive(manifest: dict, host: str) -> dict:
    tag = manifest.get('tag', '')
    if (manifest.get('schema') != RELEASE_SCHEMA or manifest.get('channel') != 'preview' or
            not re.fullmatch(r'v[0-9]+\.[0-9]+\.[0-9]+(?:-[a-z0-9.-]+)?', tag) or
            not re.fullmatch(r'[a-f0-9]{40}', manifest.get('revision', '')) or host not in HOSTS):
        raise ValueError('invalid published Linux preview manifest or host')
    archive = manifest.get('archives', {}).get(host, {})
    if (archive.get('file') != f'objc3-sdk-{tag}-{host}.tar.zst' or archive.get('target') != HOSTS[host][1] or
            type(archive.get('bytes')) is not int or archive['bytes'] <= 0 or not archive.get('source_build_id') or
            not all(SHA256.fullmatch(archive.get(key, '')) for key in ('sha256', 'seal_sha256'))):
        raise ValueError(f'invalid published preview archive coordinates for {host}')
    return archive


def published_installs(manifest: dict, host: str, archive: dict) -> list[dict]:
    run, attempt = archive.get('qualification_run'), archive.get('qualification_attempt')
    rows = archive.get('clean_installs', [])
    if (type(run) is not int or run <= 0 or type(attempt) is not int or attempt <= 0 or
            not isinstance(rows, list) or len(rows) != len(DISTRIBUTIONS) or
            any(not isinstance(row, dict) for row in rows) or
            {row.get('distribution') for row in rows} != set(DISTRIBUTIONS)):
        raise ValueError(f'{host} published preview omitted a clean-install run or distribution')
    by_distribution = {row['distribution']: row for row in rows}
    for distribution, row in by_distribution.items():
        validate_clean_install(row, host=host, distribution=distribution, revision=manifest['revision'],
                               run=run, attempt=attempt, archive=archive)
    return [by_distribution[distribution] for distribution in DISTRIBUTIONS]


def _container(reference: str) -> tuple[str, str]:
    subprocess.run(['docker', 'pull', reference], check=True)
    info, = json.loads(subprocess.check_output(['docker', 'image', 'inspect', reference]))
    image = next((value for value in info['RepoDigests'] if IMAGE.fullmatch(value)), None)
    if image is None or not IMAGE_ID.fullmatch(info['Id']):
        raise ValueError(f'Docker did not resolve an immutable image for {reference}')
    return image, info['Id']


def _context(host: str, distribution: str) -> dict:
    return {'host': host, 'distribution': distribution, 'revision': os.environ['GITHUB_SHA'],
            'run': int(os.environ['GITHUB_RUN_ID']), 'attempt': int(os.environ['GITHUB_RUN_ATTEMPT'])}


def _save(output: Path, report: dict) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + '\n')


def qualify(download: Path, output: Path, distribution: str, *, host: str, scope='full') -> None:
    if scope not in ('full', *SCOPES):
        raise ValueError('unknown clean-install scope')
    report = {'schema': SCOPE_SCHEMA if scope != 'full' else SCHEMA,
              **_context(host, distribution), 'scope': scope}
    output.parent.mkdir(parents=True, exist_ok=True)
    log = output.with_name('install.log')
    started = time.monotonic()
    try:
        archive, = download.rglob('*.tar.zst')
        record = json.loads(archive.with_suffix('.json').read_bytes())
        if record['host'] != host:
            raise ValueError('archive receipt differs from the native qualification host')
        report.update({key: record[key] for key in ('archive_sha256', 'seal_sha256', 'source_build_id')})
        image, image_id = _container(distribution)
        report.update(image=image, image_id=image_id)
        command = ['docker', 'run', '--rm', '--security-opt', 'seccomp=unconfined',
                   '--mount', f'type=bind,source={download.resolve()},target=/release,readonly',
                   image, 'sh', '/release/scripts/qualify_linux_archive.sh',
                   '/release/' + archive.relative_to(download).as_posix(),
                   record['archive_sha256'], '/opt/objc3c', scope]
        with log.open('wb') as transcript, subprocess.Popen(
                command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as process:
            for line in process.stdout:
                transcript.write(line)
                transcript.flush()
                sys.stdout.buffer.write(line)
                sys.stdout.buffer.flush()
            if process.wait():
                raise subprocess.CalledProcessError(process.returncode, command)
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        raise
    finally:
        report['elapsed_seconds'] = round(time.monotonic() - started, 3)
        if log.is_file():
            report['log_sha256'] = hashlib.sha256(log.read_bytes()).hexdigest()
        _save(output, report)


def aggregate(directory: Path, output: Path, distribution: str, *, host: str) -> None:
    context = _context(host, distribution)
    rows = {}
    for path in directory.glob('*/result.json'):
        data = path.read_bytes()
        row = json.loads(data)
        if row.get('distribution') != distribution:
            continue
        scope = row.get('scope')
        if (scope not in SCOPES or scope in rows or row.get('schema') != SCOPE_SCHEMA or
                row.get('status') != 'PASS' or any(row.get(key) != value for key, value in context.items()) or
                not IMAGE.fullmatch(row.get('image', '')) or not IMAGE_ID.fullmatch(row.get('image_id', '')) or
                not all(re.fullmatch('[a-f0-9]{64}', row.get(key, ''))
                        for key in ('archive_sha256', 'seal_sha256')) or not row.get('source_build_id') or
                hashlib.sha256(path.with_name('install.log').read_bytes()).hexdigest() != row.get('log_sha256')):
            raise ValueError(f'invalid or duplicate {distribution} clean-install scope: {scope}')
        rows[scope] = (row, hashlib.sha256(data).hexdigest())
    if set(rows) != set(SCOPES):
        raise ValueError(f'{distribution} clean-install scope inventory is incomplete: {sorted(set(SCOPES)-rows.keys())}')
    first = rows[SCOPES[0]][0]
    identity = {key: first[key] for key in ('image', 'image_id', 'archive_sha256',
                                           'seal_sha256', 'source_build_id')}
    if any(any(row[key] != value for key, value in identity.items()) for row, _ in rows.values()):
        raise ValueError(f'{distribution} clean-install scopes used different images or SDK archives')
    _save(output, {'schema': SCHEMA, 'status': 'PASS', **context, **identity,
                   'scopes': {scope: rows[scope][1] for scope in SCOPES}})


def admit_rehearsal(bundle: Path, host: str) -> tuple[dict, list[dict]]:
    manifest_path = bundle / 'linux-release.json'
    if manifest_path.is_symlink() or not manifest_path.is_file():
        raise ValueError('draft rehearsal needs a regular release manifest')
    manifest_bytes = manifest_path.read_bytes()
    manifest = json.loads(manifest_bytes)
    record = published_archive(manifest, host)
    tests = published_installs(manifest, host, record)
    archive = bundle / record['file']
    if archive.is_symlink() or not archive.is_file() or archive.stat().st_size != record['bytes']:
        raise ValueError('draft rehearsal archive length differs from the published bytes')
    if file_hash(archive) != record['sha256']:
        raise ValueError('draft rehearsal archive SHA-256 differs from the published manifest')

    decoder = shutil.which('zstd')
    if decoder is None:
        raise ValueError('draft rehearsal needs zstd to read the qualified archive admission')
    admission = read_archive_admission(archive, Path(decoder))
    if admission is None:
        raise ValueError('draft rehearsal archive is missing its sealed admission')
    sdk, _ = admission
    sealed = {key: value for key, value in sdk.items() if key != 'seal_sha256'}
    if (sdk.get('schema') != SDK_SCHEMA or sdk.get('seal_sha256') != _digest(sealed) or
            any(sdk.get(key) != record[key] for key in ('seal_sha256', 'source_build_id', 'target'))):
        raise ValueError('draft rehearsal SDK admission differs from the declared seal, source build or target')

    expected = {'linux-release.json': hashlib.sha256(manifest_bytes).hexdigest(), archive.name: record['sha256']}
    for name in INSTALLER_COMPANIONS:
        companion = bundle / name
        member = sdk['files'].get(f'share/objc3c/{name}', {})
        if (companion.is_symlink() or not companion.is_file() or
                member.get('kind') != 'file' or file_hash(companion) != member.get('sha256')):
            raise ValueError(f'draft rehearsal installer companion differs from the qualified SDK: {name}')
        expected[name] = member['sha256']

    checksum_path = bundle / 'SHA256SUMS'
    if (checksum_path.is_symlink() or not checksum_path.is_file() or
            not 0 < checksum_path.stat().st_size <= 64 * 1024):
        raise ValueError('draft rehearsal needs its complete bounded checksum index')
    checksums = {}
    for line in checksum_path.read_text().splitlines():
        row = re.fullmatch(r'([a-f0-9]{64})  ([A-Za-z0-9][A-Za-z0-9._-]*)', line)
        if row is None or row[2] in checksums:
            raise ValueError('draft rehearsal checksum index has an invalid or duplicate entry')
        checksums[row[2]] = row[1]
    if any(checksums.get(name) != signature for name, signature in expected.items()):
        raise ValueError('draft rehearsal checksum index omitted or changed a required asset')
    return record, tests


def rehearse(bundle: Path, host: str) -> None:
    host_identity(host, require_native=True)
    record, tests = admit_rehearsal(bundle, host)
    for test in tests:
        image, image_id = _container(test['image'])
        if image != test['image'] or image_id != test['image_id']:
            raise ValueError('draft rehearsal image differs from clean-install qualification')
        subprocess.run(['docker', 'run', '--rm',
                        '--mount', f'type=bind,source={bundle.resolve()},target=/release,readonly',
                        '--workdir', '/release', image, 'sh', '-eu', '-c', '''
sha256sum --check --ignore-missing SHA256SUMS
archive=$1
sh install-linux.sh --provision --archive "$archive" --sha256 "$2" --root /opt/objc3c
. /opt/objc3c/activate.sh
python3 - "$OBJC3C_SDK_ROOT/objc3-sdk.json" "$3" "$4" <<'PY'
import json
import sys

with open(sys.argv[1]) as stream:
    installed = json.load(stream)
if (installed.get('seal_sha256'), installed.get('source_build_id')) != tuple(sys.argv[2:]):
    sys.exit('activated SDK differs from the declared release seal or source build')
PY
objc3c --version
objc3c "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/main.objc3" -O2 -o /tmp/application
/tmp/application
sh "$OBJC3C_SDK_ROOT/share/objc3c/examples/log-summary/verify.sh"
''', 'sh', record['file'], record['sha256'], record['seal_sha256'], record['source_build_id']], check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', required=True)
    parser.add_argument('--directory', type=Path, required=True)
    parser.add_argument('--distribution', choices=DISTRIBUTIONS)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--scope', choices=SCOPES)
    parser.add_argument('--aggregate', action='store_true')
    args = parser.parse_args()
    if args.distribution and args.output:
        if args.aggregate and args.scope:
            parser.error('supply either a single scope or the complete scope aggregate')
        if args.aggregate:
            aggregate(args.directory, args.output, args.distribution, host=args.host)
        else:
            qualify(args.directory, args.output, args.distribution, host=args.host, scope=args.scope or 'full')
    elif not args.distribution and not args.output and not args.scope and not args.aggregate:
        rehearse(args.directory, args.host)
    else:
        parser.error('supply distribution and output together for qualification')


if __name__ == '__main__':
    main()
