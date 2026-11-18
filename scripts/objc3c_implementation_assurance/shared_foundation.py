"""Transport the exact compiler and native build inputs shared by analysis partitions."""
from __future__ import annotations

import json
import os
from pathlib import Path
import posixpath
import shutil
import subprocess
import tarfile

from scripts.objc3c_assurance.evidence import artifact, native_files
from scripts.objc3c_assurance.instrumented_foundation import (
    PROFILE, _identity, _portable_identity, _retain_foundation_evidence,
)
from scripts.objc3c_toolchain.binary_packages import digest, write_json
from scripts.objc3c_toolchain.build import _build_foundation, foundation_lock
from scripts.objc3c_toolchain.core import build_plans, prepare, seal_development
from scripts.objc3c_toolchain.implementation_analysis import dependencies, inventory, patch_units
from scripts.objc3c_toolchain.linux_host import native_linux_host, require_elf

SCHEMA = 'objc3c.implementation-analysis-foundation.v2'
TABLEGEN = Path('bin/llvm-tblgen')


def _tablegen(build: Path) -> Path:
    path = build / TABLEGEN
    if not path.is_file() or path.is_symlink() or not os.access(path, os.X_OK):
        raise ValueError('shared analysis foundation requires its own executable llvm-tblgen')
    require_elf(path)
    return path


def _build_files(repo: Path, configuration: dict) -> set[Path]:
    build = Path(configuration['paths']['build'])
    source = Path(configuration['paths']['source'])
    ninja = configuration['host_environment']['ninja_path']
    database = build / 'compile_commands.json'
    graph = subprocess.check_output([ninja, '-C', build, '-t', 'commands'], text=True)
    rows = inventory(database, graph, {source / name for name in patch_units(repo)})
    outputs = {row['output'] for row in rows}
    recorded = dependencies(subprocess.check_output([ninja, '-C', build, '-t', 'deps',
        *(str(Path(path).relative_to(build)) for path in sorted(outputs))], text=True),
        build=build, selected_outputs=outputs)

    paths = {
        database, build / 'CMakeCache.txt', build / 'build.ninja', build / '.ninja_deps',
        build / 'foundation-build-report.json', Path(configuration['configuration_trace']),
        _tablegen(build),
    }
    # Ninja checks the recorded object's nanosecond mtime to authenticate its
    # dependency closure. Retain only the compiled objects that own those rows.
    paths.update(map(Path, recorded))
    paths.update(path for path in build.rglob('*')
                 if path.is_file() and path.suffix in ('.ninja', '.modmap', '.dd'))
    paths.update(path for row in rows for name in row['response_files']
                 if (path := Path(name)).is_relative_to(build))
    paths.update(path for included in recorded.values() for name in included
                 if (path := Path(name)).is_relative_to(build))
    paths.update(path for root in (build / 'include', build / 'tools/clang/include')
                 for path in root.rglob('*') if path.is_file())
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        alias = build / 'lib' / name
        paths.add(alias)
        while alias.is_symlink():
            alias = alias.parent / alias.readlink()
            if not alias.resolve(strict=True).is_relative_to(build):
                raise ValueError('analysis build library escaped its compiled foundation')
            paths.add(alias)
    return paths


def _archive(repo: Path, archive: Path, allowed: tuple[Path, ...], files: set[Path]) -> dict:
    members = []
    for path in sorted(files):
        if (not (path.is_file() or path.is_symlink()) or not any(path.is_relative_to(root) and
                path.resolve(strict=True).is_relative_to(root) for root in allowed)):
            raise ValueError('analysis foundation contains an unowned file: '+str(path))
        members.append(path.relative_to(repo).as_posix())
    subprocess.run(['tar', '--format=pax', '--no-recursion', '--null', '-I', 'zstd -T2 -3',
                    '-cf', archive, '-C', repo, '-T', '-'],
                   input=b'\0'.join(name.encode() for name in members) + b'\0', check=True)
    prefixes = tuple(root.relative_to(repo).as_posix() + '/' for root in allowed)
    if _archive_members(archive, prefixes) != members:
        raise ValueError('analysis foundation archive differs from its selected compiler inputs')
    return {'sha256': digest(archive), 'bytes': archive.stat().st_size, 'members': members}


def _archive_members(archive: Path, prefixes: tuple[str, ...]) -> list[str]:
    names = []
    with subprocess.Popen(['zstd', '-dc', archive], stdout=subprocess.PIPE) as decoded:
        with tarfile.open(fileobj=decoded.stdout, mode='r|') as members:
            for member in members:
                name = member.name
                if (not name.startswith(prefixes) or '..' in Path(name).parts or
                        not (member.isfile() or member.issym() or member.islnk())):
                    raise ValueError('analysis foundation archive contains an unowned member')
                if member.issym() or member.islnk():
                    target = (posixpath.join(posixpath.dirname(name), member.linkname)
                              if member.issym() else member.linkname)
                    if not posixpath.normpath(target).startswith(prefixes):
                        raise ValueError('analysis foundation archive contains an unowned link')
                names.append(name)
        # Read the stream trailer before checking zstd's verification result.
        while decoded.stdout.read(1024 * 1024):
            pass
        if decoded.wait():
            raise ValueError('analysis foundation archive decompression failed')
    return names


def produce(repo: Path, plan: dict, output: Path) -> dict:
    from .runner import owned_path
    from .plan import from_checkout

    repo = repo.resolve(strict=True)
    output = owned_path(repo, output)
    development = owned_path(repo, repo / 'out/toolchains/objc3-development')
    if output.is_relative_to(development) or development.is_relative_to(output):
        raise ValueError('analysis foundation evidence overlaps its build products')
    if not plan['required'] or from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
        raise ValueError('analysis foundation requires its exact selected implementation revision')
    output.mkdir(parents=True, exist_ok=False)
    evidence = output / 'foundation'
    evidence.mkdir()
    report = {'schema': SCHEMA, 'status': 'RUNNING', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'host': native_linux_host(), 'workspace': str(repo)}
    configuration = None
    try:
        owned_path(repo, development / '.foundation-build.lock')
        with foundation_lock(repo):
            foundation = _build_foundation(repo, host=native_linux_host(), fetch=True, stage=True,
                                          instrumentation_profile=PROFILE, test_results=evidence)
            configuration = build_plans(repo, host=native_linux_host(), instrumentation_profile=PROFILE)['configure']
            portable = _portable_identity(repo)
            if foundation['status'] != 'passed' or foundation['source_identity'] != portable['source_build_id']:
                raise ValueError('shared analysis foundation did not complete qualification')
            stage, build = (Path(configuration['paths'][key]).resolve(strict=True) for key in ('stage', 'build'))
            files = _build_files(repo, configuration)
            files.update(path for path in stage.rglob('*') if path.is_file() or path.is_symlink())
            _retain_foundation_evidence(repo, configuration, evidence)
            report.update(status='PASS', identity=_identity(portable, stage),
                          foundation_artifacts=[artifact(evidence, path) for path in native_files(evidence)],
                          archive=_archive(repo, output / 'foundation.tar.zst', (stage, build), files))
        return report
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        _retain_foundation_evidence(repo, configuration, evidence)
        raise
    finally:
        write_json(output / 'producer.json', report)


def restore(repo: Path, plan: dict, bundle: Path, evidence: Path) -> tuple[dict, dict]:
    archive = bundle / 'foundation.tar.zst'
    receipt = bundle / 'producer.json'
    artifact(bundle, receipt)
    report = json.loads(receipt.read_text())
    archive_identity = artifact(bundle, archive)
    portable = _portable_identity(repo)
    stage = Path(portable['paths']['stage'])
    build = repo / 'out/toolchains/objc3-development' / ('build-' + native_linux_host() + '-' + PROFILE)
    source = build.parent / 'src'
    locations = tuple(str(path.relative_to(repo)) + '/' for path in (stage, build))
    if (report.get('schema') != SCHEMA or report.get('status') != 'PASS' or
            report.get('plan_sha256') != plan['sha256'] or report.get('revision') != plan['revision'] or
            report.get('host') != native_linux_host() or report.get('workspace') != str(repo) or
            report.get('identity', {}).get('source_build_id') != portable['source_build_id'] or
            report.get('archive', {}).get('bytes') != archive_identity['bytes'] or
            report['archive'].get('sha256') != archive_identity['sha256'] or
            stage.exists() or build.exists() or source.exists()):
        raise ValueError('analysis foundation bundle differs from the selected revision or host')
    files = _archive_members(archive, locations)
    if not files or files != report['archive']['members'] or len(files) != len(set(files)):
        raise ValueError('analysis foundation archive has escaped or incomplete members')
    if (build / TABLEGEN).relative_to(repo).as_posix() not in files:
        raise ValueError('analysis foundation archive omitted llvm-tblgen')
    original = bundle / 'foundation'
    reports = native_files(original)
    if not reports or [artifact(original, path) for path in reports] != report['foundation_artifacts']:
        raise ValueError('analysis foundation diagnostic bytes differ from the producer receipt')

    prepare(repo, source_dir=source, fetch=True)
    seal_development(repo, source_dir=source)
    subprocess.run(['tar', '-I', 'zstd', '-xf', archive, '-C', repo], check=True)
    _tablegen(build)
    if _identity(portable, stage) != report['identity']:
        raise ValueError('restored analysis compiler differs from the producer')
    evidence.mkdir(parents=True, exist_ok=False)
    for path in reports:
        target = evidence / path.relative_to(original)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(path, target)
    shutil.copyfile(receipt, evidence / 'shared-producer.json')
    configuration = build_plans(repo, host=native_linux_host(), instrumentation_profile=PROFILE)['configure']
    return configuration, report
