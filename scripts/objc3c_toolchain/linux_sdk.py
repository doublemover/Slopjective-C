"""One measured Linux SDK pipeline: foundation, runtime, frontend, evidence, release."""
from __future__ import annotations
from datetime import datetime, timezone
import fcntl
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time

from .build import build_foundation
from .core import build_plans, verify_development
from .facade import facade_identity
from .install_sdk import install
from .package import inventory, package_sdk, verify_sdk
from .qualify_installed import qualify_installed
from .sdk_example import write_json


PROJECT_SDK_PREFIXES = ('bin/objc3', 'bin/clang-scan-deps', 'lib/libobjc',
                        'lib/cmake/objc3c/', 'include/objc/', 'include/objc3c/',
                        'include/runtime/', 'share/objc3c/')


def is_project_sdk_path(name: str) -> bool:
    return name == 'objc3-sdk.json' or any(
        name == prefix.rstrip('/') or name.startswith(prefix)
        for prefix in PROJECT_SDK_PREFIXES)


def copy_foundation(sdk: Path, prefix: Path) -> None:
    def ignore(directory, names):
        relative = Path(directory).relative_to(sdk)
        return [name for name in names if name == '__pycache__' or
                is_project_sdk_path((relative / name).as_posix())]
    # Reinstall every current project component; a cached SDK must never
    # resurrect a removed compiler header, library, example or executable.
    shutil.copytree(sdk, prefix, symlinks=True, ignore=ignore)


def foundation_identity(sdk: Path) -> str:
    records = inventory(sdk)
    # Project frontend/runtime/sample changes do not invalidate the LLVM cache.
    selected = {name: record for name, record in records.items()
                if record['kind'] != 'directory' and name.startswith(('bin/', 'lib/', 'include/'))
                and not is_project_sdk_path(name)}
    return hashlib.sha256(json.dumps(selected, sort_keys=True).encode()).hexdigest()


def input_identity(repo: Path, sdk: Path) -> dict:
    files = set()
    for directory in ('scripts/objc3c_toolchain', 'scripts/objc3c_toolchain_runtime',
                      'scripts/objc3c_native_validation', 'tests/clang_foundation',
                      'tests/tooling/fixtures/clang_foundation', 'native/objc3c/src/runtime'):
        files.update(path for path in (repo / directory).rglob('*') if path.is_file()
                     and '__pycache__' not in path.parts and path.suffix not in ('.pyc',))
    # Qualification owners, including editor fixtures and shared executor code,
    # are authoritative inputs to release reuse rather than a second file list.
    from scripts.objc3c_native_validation.plan import build_plan
    qualification = build_plan(repo, drivers=('clang', 'objc3c-clang'))
    for node in qualification['selected_nodes']:
        files.update(repo / name for name in node['identity_inputs'])
    files.add(repo / 'scripts/install_objc3c_linux.sh')
    files.add(repo / 'out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen')
    inputs = {path.relative_to(repo).as_posix(): hashlib.sha256(path.read_bytes()).hexdigest()
              for path in sorted(files)}
    payload = {'foundation_contents': foundation_identity(sdk), 'foundation_plan': build_plans(repo, host='linux-x86_64')['configure']['identity'],
               'frontend': facade_identity(repo)['sha256'], 'inputs': inputs}
    return {'sha256': hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest(), **payload}


def build_sdk(repo: Path, *, fetch: bool = False, install_root: Path | None = None, foundation_sdk: Path | None = None) -> dict:
    repo = repo.resolve()
    base = repo / 'out/toolchains/linux-sdk-builds'
    base.mkdir(parents=True, exist_ok=True)
    with (base / '.pipeline.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        return _build_sdk(repo, base, fetch, install_root or repo / 'out/toolchains/linux-sdk', foundation_sdk)


def _install_qualified_archive(report: dict, directory: Path, install_root: Path,
                              *, expected_identity: str) -> dict:
    """Publish an already qualified archive without retaining a second SDK copy."""
    if (report.get('status') != 'awaiting-installation' or
            report.get('input_identity') != expected_identity or
            report.get('build_prefix') != str(directory / 'prefix')):
        raise ValueError('qualified archive does not match the pending build identity')
    archive = Path(report['archive']).resolve(strict=True)
    with archive.open('rb') as stream:
        if hashlib.file_digest(stream, 'sha256').hexdigest() != report['archive_sha256']:
            raise ValueError('qualified archive checksum differs; candidate preserved')
    prefix = directory / 'prefix'
    if prefix.is_symlink() or (prefix.exists() and not prefix.is_dir()):
        raise ValueError('build candidate is not an owned directory; preserving it')
    # Persist the successful qualification before reclamation. A process may
    # stop during deletion or extraction; the next invocation resumes from
    # this exact archive without compiling or qualifying the product again.
    write_json(directory / 'qualified-archive.json', report)
    if prefix.exists():
        shutil.rmtree(prefix)
    report['build_candidate_reclaimed'] = True
    report['installed'] = install(archive, report['archive_sha256'], install_root)
    return report


def _build_sdk(repo: Path, base: Path, fetch: bool, install_root: Path, foundation_sdk: Path | None) -> dict:
    started = time.monotonic()
    # Pin the bootstrap before activating or installing a product SDK. A product
    # build must never silently replace the compiler which builds itself.
    bootstrap = os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT') or os.environ.get('OBJC3C_LLVM_ROOT') or os.environ.get('LLVM_ROOT')
    if not bootstrap:
        raise ValueError('set OBJC3C_BOOTSTRAP_LLVM_ROOT to the pinned bootstrap LLVM SDK')
    os.environ['OBJC3C_BOOTSTRAP_LLVM_ROOT'] = str(Path(bootstrap).resolve())
    sdk = repo / 'out/toolchains/objc3-development/stage-linux-x86_64'
    if foundation_sdk is None and (install_root / 'current/objc3-sdk.json').is_file():
        active = (install_root / 'current').resolve()
        if json.loads((active / 'objc3-sdk.json').read_text())['source_build_id'] == verify_development(repo)['build_id']:
            foundation_sdk = active
    if foundation_sdk is not None:
        sdk = foundation_sdk.resolve()
        cached = verify_sdk(sdk)
        if cached['source_build_id'] != verify_development(repo)['build_id']:
            raise ValueError('cached SDK foundation differs from the current source and build configuration')
        foundation = {'status': 'passed', 'cache_hit': True, 'prefix': str(sdk),
                      'source_identity': cached['source_build_id'], 'seal_sha256': cached['seal_sha256']}
    else:
        foundation = build_foundation(repo, host='linux-x86_64', fetch=fetch, stage=True)
    identity = input_identity(repo, sdk)
    directory = base / identity['sha256']
    directory.mkdir(exist_ok=True)
    receipt_path = directory / 'release.json'
    if receipt_path.exists():
        receipt = json.loads(receipt_path.read_text())
        # Publication makes the checked archive authoritative. The mutable
        # candidate may have been reclaimed or edited after release.
        installed = install(Path(receipt['archive']), receipt['archive_sha256'], install_root)
        return {**receipt, 'installed': installed, 'cache_hit': True,
                'elapsed_seconds': round(time.monotonic() - started, 6)}
    pending = directory / 'qualified-archive.json'
    if pending.is_file():
        report = json.loads(pending.read_text())
        try:
            report = _install_qualified_archive(report, directory, install_root,
                                                expected_identity=identity['sha256'])
            report.update(status='PASS', resumed_qualified_archive=True,
                          elapsed_seconds=round(time.monotonic() - started, 6))
            write_json(receipt_path, report)
            return {**report, 'cache_hit': False}
        except Exception as error:
            report.update(status='FAIL', error=str(error))
            raise
        finally:
            write_json(directory / 'build-report.json', report)
    prefix = directory / 'prefix'
    if prefix.exists():
        raise ValueError(f'incomplete SDK build preserved at {directory}; inspect it before removing it and retrying')
    # This is a mutable build candidate. Only a sealed archive is published to
    # the immutable installation store after all consumers have passed.
    copy_foundation(sdk, prefix)
    report = {'status': 'running', 'input_identity': identity['sha256'], 'build_prefix': str(prefix),
              'foundation': foundation, 'commands': []}
    env = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib'), 'PYTHONDONTWRITEBYTECODE': '1',
           'CCACHE_COMPILERCHECK': 'content'}
    def run(name, command):
        print(f'Linux SDK {name}: {directory / (name + ".log")}', flush=True)
        step_started = time.monotonic()
        with (directory / f'{name}.log').open('w') as log:
            result = subprocess.run([str(arg) for arg in command], cwd=repo, env=env,
                                    stdout=log, stderr=subprocess.STDOUT)
        report['commands'].append({'name': name, 'command': [str(arg) for arg in command],
            'exit_code': result.returncode, 'elapsed_seconds': round(time.monotonic() - step_started, 6)})
        if result.returncode:
            raise ValueError(f'Linux SDK {name} failed; see {directory / (name + ".log")}')
    try:
        runtime = repo / 'out/toolchains/libobjc2-v2.3'
        run('runtime', [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain_runtime.build_linux',
            '--repo', repo, '--compiler-root', sdk, '--report-file', directory / 'runtime-report.json',
            *(['--fetch'] if fetch else [])])
        runtime_report = json.loads((directory / 'runtime-report.json').read_text())
        runtime_stage = Path(runtime_report['stage'])
        clients = runtime / 'client-tests-linux' / runtime_report['build_key']
        run('runtime-configure', ['cmake', '-S', repo / 'tests/clang_foundation/native_runtime',
            '-B', clients, '-G', 'Ninja', f'-DCMAKE_C_COMPILER={sdk / "bin/clang"}',
            f'-DCMAKE_CXX_COMPILER={sdk / "bin/clang++"}', f'-DCMAKE_OBJC_COMPILER={sdk / "bin/clang"}',
            f'-DCMAKE_OBJCXX_COMPILER={sdk / "bin/clang++"}',
            f'-DOBJC3_LIBOBJC2_SOURCE_DIR={runtime / "src"}',
            f'-DOBJC3_LIBOBJC2_BUILD_DIR={runtime_stage}',
            f'-DOBJC3_ROBIN_MAP_SOURCE_DIR={runtime_report["robin_map_source"]}', '-DBUILD_TESTING=ON'])
        run('runtime-clients', ['cmake', '--build', clients, '--parallel', '2'])
        run('runtime-tests', ['ctest', '--test-dir', clients, '--no-tests=error',
                              '--output-on-failure', '--timeout', '30', '--stop-on-failure'])
        run('runtime-install', ['cmake', '--install', clients, '--prefix', prefix])
        frontend = repo / 'out/toolchains/objc3-facade/build-linux-x86_64'
        configure = ['cmake', '-S', repo / 'native/objc3c', '-B', frontend, '-G', 'Ninja',
            f'-DOBJC3C_CLANG_FOUNDATION_ROOT={sdk}', f'-DCMAKE_CXX_COMPILER={Path(bootstrap) / "bin/clang++"}',
            f'-DPython3_EXECUTABLE={sys.executable}', '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', '-DBUILD_TESTING=ON']
        ccache = shutil.which('ccache')
        if ccache:
            configure.append(f'-DCMAKE_CXX_COMPILER_LAUNCHER={ccache}')
        run('frontend-configure', configure)
        jobs = build_plans(repo, host='linux-x86_64')['configure']['host_environment']['resource_limits']['compile_jobs']
        run('frontend-build', ['cmake', '--build', frontend, '--parallel', str(jobs)])
        run('frontend-tests', ['ctest', '--test-dir', frontend, '--no-tests=error', '--output-on-failure'])
        run('frontend-install', ['cmake', '--install', frontend, '--prefix', prefix])
        report['installed_qualification'] = qualify_installed(repo, prefix, directory / 'installed-qualification')
        env['OBJC3C_CLANG_FOUNDATION_ROOT'] = str(prefix)
        run_id = 'linux-sdk-' + datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
        run('native-validation', [sys.executable, '-B', '-m', 'scripts.objc3c_native_validation',
                                 '--repo', repo, '--run-id', run_id])
        if input_identity(repo, sdk) != identity:
            raise ValueError('SDK inputs changed during qualification; candidate has not been published')
        receipt = package_sdk(repo, prefix, repo / 'out/native-validation' / run_id / 'summary.json',
                              repo / 'out/toolchains/sdk-cache')
        # Packaging now runs substantial installed-source qualification. Bind
        # its scripts and fixtures through publication, not just through the
        # preceding native validation batch.
        if input_identity(repo, sdk) != identity:
            raise ValueError('SDK inputs changed during packaging; candidate has not been published')
        report.update(receipt, status='awaiting-installation')
        _install_qualified_archive(report, directory, install_root,
                                   expected_identity=identity['sha256'])
        report.update(status='PASS', resumed_qualified_archive=False,
                      elapsed_seconds=round(time.monotonic() - started, 6))
        receipt_path.write_text(json.dumps(report, indent=2) + '\n')
        return {**report, 'cache_hit': False}
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        (directory / 'build-report.json').write_text(json.dumps(report, indent=2) + '\n')
