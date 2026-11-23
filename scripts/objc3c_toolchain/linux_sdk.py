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

from .linux_host import native_linux_host, native_target
from .build import build_foundation
from .core import FoundationConfigurationChanged, build_plans, verify_development
from .facade import facade_identity
from .frontend_tests import qualify as qualify_frontend_tests
from .install_sdk import install
from .package import _archive_sdk, inventory, package_sdk, verify_sdk
from .qualify_installed import qualify_installed
from .sdk_example import write_json
from .native_headers import discover as discover_native_headers, materialize as materialize_native_headers


# FileCheck is added by project packaging, with its bytes bound separately in
# input_identity. It must not mutate the reusable LLVM foundation signature.
PROJECT_SDK_PREFIXES = ('bin/objc3', 'bin/clang-scan-deps', 'bin/FileCheck', 'lib/libobjc',
                        'lib/cmake/objc3c/', 'include/objc/', 'include/objc3c/',
                        'include/objc3/', 'include/runtime/', 'share/objc3c/')
NATIVE_TEST_INPUT_PREFIXES = ('native/objc3c/tests/', 'tests/clang_foundation/',
                              'tests/tooling/fixtures/clang_foundation/')


def is_project_sdk_path(name: str) -> bool:
    parts = Path(name).parts
    compiler_runtimes = (len(parts) >= 5 and parts[:2] == ('lib', 'clang') and parts[3] == 'lib')
    native_headers = len(parts) >= 4 and parts[:2] == ('lib', 'clang') and parts[3] == 'objc3-native-headers'
    return compiler_runtimes or native_headers or name in ('objc3-sdk.json', 'bin/clang.cfg', 'bin/clang++.cfg') or any(
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


def adopt_candidate_foundation(repo: Path, source: Path, prefix: Path,
                               *, expected_contents: str) -> dict:
    """Use the verified candidate copy as the build SDK and retire its stage."""
    signature = foundation_identity(source)
    if signature != expected_contents or foundation_identity(prefix) != signature:
        raise ValueError('candidate foundation differs from its source; preserving staging outputs')
    stage = repo / ('out/toolchains/objc3-development/stage-' + native_linux_host())
    reclaimed = source.absolute() == stage.absolute() and not source.is_symlink()
    result = {'source': str(source), 'prefix': str(prefix),
              'foundation_contents': signature, 'duplicate_stage_reclaimed': reclaimed}
    if reclaimed:
        shutil.rmtree(source)
    return result


def _qualification_input_path(repo: Path, path: Path) -> Path:
    try:
        resolved = path.resolve(strict=True)
    except (OSError, RuntimeError) as error:
        raise ValueError(f'qualification input cannot be resolved: {path}') from error
    if not resolved.is_relative_to(repo):
        raise ValueError(f'qualification input escapes the checkout: {path}')
    return resolved


def input_identity(repo: Path, sdk: Path) -> dict:
    repo = repo.resolve(strict=True)
    files = set()
    for directory in ('scripts/objc3c_toolchain', 'scripts/objc3c_toolchain_runtime',
                      'scripts/objc3c_native_validation', 'tests/clang_foundation',
                      'tests/tooling/fixtures/clang_foundation', 'native/objc3c/src/runtime', 'native/objc3c/tests',
                      'stdlib/include', 'showcase/log-summary'):
        root = repo / directory
        if not (root.exists() or root.is_symlink()):
            continue
        pending = [(root, frozenset())]
        while pending:
            path, ancestors = pending.pop()
            if path.name == '__pycache__' or path.suffix == '.pyc':
                continue
            resolved = _qualification_input_path(repo, path)
            if resolved.is_dir():
                if resolved in ancestors:
                    raise ValueError(f'qualification input directory cycle: {path}')
                lineage = ancestors | {resolved}
                pending.extend((path / child.name, lineage) for child in resolved.iterdir())
            elif resolved.is_file():
                files.add(path)
            else:
                raise ValueError(f'qualification input must be a regular file: {path}')
    # Qualification owners, including editor fixtures and shared executor code,
    # are authoritative inputs to release reuse rather than a second file list.
    from scripts.objc3c_native_validation.plan import build_plan
    qualification = build_plan(repo, drivers=('clang', 'objc3c-clang'))
    for node in qualification['selected_nodes']:
        files.update(repo / name for name in node['identity_inputs'])
    files.add(repo / 'scripts/install_objc3c_linux.sh')
    files.add(repo / 'stdlib/module_inventory.json')
    from .package import distribution_license
    license_file = distribution_license(repo)
    if license_file is not None:
        files.add(license_file)
    files.add(repo / 'toolchains/linux-hosts.json')
    files.add(repo / ('out/toolchains/objc3-development/build-' + native_linux_host()) / 'bin/llvm-tblgen')
    files.add(repo / ('out/toolchains/objc3-development/build-' + native_linux_host()) / 'bin/FileCheck')
    inputs, input_resolution = {}, {}
    for path in sorted(files):
        resolved = _qualification_input_path(repo, path)
        if not resolved.is_file():
            raise ValueError(f'qualification input must be a regular file: {path}')
        relative = path.relative_to(repo).as_posix()
        with resolved.open('rb') as stream:
            inputs[relative] = hashlib.file_digest(stream, 'sha256').hexdigest()
        if resolved != path:
            input_resolution[relative] = resolved.relative_to(repo).as_posix()
    configure = build_plans(repo, host=native_linux_host())['configure']
    # The build plan reports mutable output inventories for consumers. Release
    # reuse binds the actual foundation contents separately; reclaiming its
    # duplicate stage must not change the source/configuration identity.
    foundation_inputs = {key: value for key, value in configure.items()
                         if key not in ('identity', 'sdk_consumers')}
    foundation_plan = hashlib.sha256(json.dumps(foundation_inputs, sort_keys=True).encode()).hexdigest()
    native_headers = discover_native_headers(sdk / 'bin/clang')
    from .compiler_runtimes import discover as discover_runtimes
    runtimes = discover_runtimes(repo, Path(os.environ['OBJC3C_BOOTSTRAP_LLVM_ROOT']))
    payload = {'foundation_contents': foundation_identity(sdk), 'foundation_plan': foundation_plan,
               'native_headers': {'sha256': native_headers['sha256']}, 'compiler_runtimes': runtimes,
               'frontend': facade_identity(repo)['sha256'], 'inputs': inputs,
               'input_resolution': input_resolution}
    test_inputs = {name: (signature, input_resolution.get(name)) for name, signature in inputs.items()
                   if name.startswith(NATIVE_TEST_INPUT_PREFIXES)}
    payload['native_test_inputs_sha256'] = hashlib.sha256(json.dumps(test_inputs, sort_keys=True).encode()).hexdigest()
    return {'sha256': hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest(), **payload}


def build_sdk(repo: Path, *, fetch: bool = False, install_root: Path | None = None, foundation_sdk: Path | None = None,
              compiler_instrumentation_profile: str | None = None) -> dict:
    repo = repo.resolve()
    base = repo / 'out/toolchains/linux-sdk-builds'
    base.mkdir(parents=True, exist_ok=True)
    with (base / '.pipeline.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        instrumentation = None
        if compiler_instrumentation_profile:
            from .compiler_instrumentation import build_and_qualify
            instrumentation = build_and_qualify(repo, profile=compiler_instrumentation_profile, fetch=fetch)
        report = _build_sdk(repo, base, fetch, install_root or repo / 'out/toolchains/linux-sdk', foundation_sdk)
        # This is evidence about a distinct compiler-process graph, not a claim
        # that the ordinary shipped archive contains instrumented libraries.
        if instrumentation is not None:
            report = {**report, 'compiler_implementation_qualification': instrumentation}
        return report


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
    sdk = repo / ('out/toolchains/objc3-development/stage-' + native_linux_host())
    development = None
    if foundation_sdk is None and (install_root / 'current/objc3-sdk.json').is_file():
        active = (install_root / 'current').resolve()
        try:
            development = verify_development(repo)
        except FoundationConfigurationChanged:
            pass
        if (development is not None and
                json.loads((active / 'objc3-sdk.json').read_text())['source_build_id'] == development['build_id']):
            foundation_sdk = active
    if foundation_sdk is not None:
        sdk = foundation_sdk.resolve()
        cached = verify_sdk(sdk)
        development = development if development is not None else verify_development(repo)
        if cached.get('target') != native_target():
            raise ValueError('cached SDK foundation targets a different machine')
        for key in ('upstream_commit', 'patch_digest'):
            if cached.get(key) != development[key]:
                raise ValueError('cached SDK foundation source differs at ' + key)
        if cached.get('build_config_digest') != development['build_config_digest']:
            raise ValueError('cached SDK foundation build configuration differs')
        if cached.get('source_build_id') != development['build_id']:
            raise ValueError('cached SDK foundation build identity differs')
        foundation = {'status': 'passed', 'cache_hit': True, 'prefix': str(sdk),
                      'source_identity': cached['source_build_id'], 'seal_sha256': cached['seal_sha256']}
    else:
        foundation = build_foundation(repo, host=native_linux_host(), fetch=fetch, stage=True)
    identity = input_identity(repo, sdk)
    directory = base / identity['sha256']
    directory.mkdir(exist_ok=True)
    receipt_path = directory / 'release.json'
    if receipt_path.exists():
        receipt = json.loads(receipt_path.read_text())
        if (not isinstance(receipt, dict) or receipt.get('status') != 'PASS' or
                receipt.get('input_identity') != identity['sha256']):
            raise ValueError('cached release receipt does not match the qualification identity')
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
    cache_dir = repo / 'out/toolchains/sdk-cache'
    checkpoint = directory / 'archive-inputs.json'
    if checkpoint.is_file():
        report = json.loads(checkpoint.read_bytes())
        receipt = report.pop('archive_inputs')
        archive = Path(receipt['archive'])
        if (report.get('input_identity') != identity['sha256'] or
                report.get('build_prefix') != str(prefix) or prefix.is_symlink() or
                archive.parent != cache_dir):
            raise ValueError('archive publication checkpoint differs from the qualified SDK build')
        verify_sdk(prefix, receipt['seal_sha256'])
        report.update(_archive_sdk(prefix, archive, receipt), status='awaiting-installation')
        if input_identity(repo, prefix) != identity:
            raise ValueError('SDK inputs changed while resuming archive publication')
        _install_qualified_archive(report, directory, install_root,
                                   expected_identity=identity['sha256'])
        report.update(status='PASS', resumed_qualified_archive=True,
                      elapsed_seconds=round(time.monotonic() - started, 6))
        write_json(receipt_path, report)
        write_json(directory / 'build-report.json', report)
        return {**report, 'cache_hit': False}
    if prefix.exists():
        raise ValueError(f'incomplete SDK build preserved at {directory}; inspect it before removing it and retrying')
    # This is a mutable build candidate. Only a sealed archive is published to
    # the immutable installation store after all consumers have passed.
    copy_foundation(sdk, prefix)
    report = {'status': 'running', 'input_identity': identity['sha256'], 'build_prefix': str(prefix),
              'foundation': foundation, 'commands': []}
    foundation_source = sdk
    sdk = prefix
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
        report['candidate_foundation'] = adopt_candidate_foundation(
            repo, foundation_source, prefix, expected_contents=identity['foundation_contents'])
        headers = discover_native_headers(prefix / 'bin/clang')
        if headers['sha256'] != identity['native_headers']['sha256']:
            raise ValueError('native standard headers changed before SDK assembly')
        report['native_standard_headers'] = materialize_native_headers(prefix, headers)
        from .compiler_runtimes import materialize as materialize_runtimes
        report['compiler_runtimes'] = materialize_runtimes(prefix, identity['compiler_runtimes'])
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
        frontend = repo / ('out/toolchains/objc3-facade/build-' + native_linux_host())
        configure = ['cmake', '-S', repo / 'native/objc3c', '-B', frontend, '-G', 'Ninja',
            f'-DOBJC3C_CLANG_FOUNDATION_ROOT={sdk}', f'-DCMAKE_CXX_COMPILER={Path(bootstrap) / "bin/clang++"}',
            f'-DOBJC3C_NATIVE_TEST_INPUT_SHA256={identity["native_test_inputs_sha256"]}',
            f'-DPython3_EXECUTABLE={sys.executable}',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DCMAKE_MODULE_LINKER_FLAGS=-fuse-ld=lld -Wl,--disable-new-dtags',
            '-DBUILD_TESTING=ON']
        ccache = shutil.which('ccache')
        if ccache:
            configure.append(f'-DCMAKE_CXX_COMPILER_LAUNCHER={ccache}')
        run('frontend-configure', configure)
        jobs = build_plans(repo, host=native_linux_host())['configure']['host_environment']['resource_limits']['compile_jobs']
        run('frontend-build', ['cmake', '--build', frontend, '--target',
                               'objc3c', 'clang-scan-deps', '--parallel', str(jobs)])
        report['frontend_test_output_reclamation'] = qualify_frontend_tests(
            frontend, directory / 'frontend-tests', run, jobs=jobs)
        run('frontend-install', ['cmake', '--install', frontend, '--prefix', prefix])
        report['installed_qualification'] = qualify_installed(repo, prefix, directory / 'installed-qualification')
        env['OBJC3C_CLANG_FOUNDATION_ROOT'] = str(prefix)
        run_id = 'linux-sdk-' + datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
        run('native-validation', [sys.executable, '-B', '-m', 'scripts.objc3c_native_validation',
                                 '--repo', repo, '--run-id', run_id])
        if input_identity(repo, sdk) != identity:
            raise ValueError('SDK inputs changed during qualification; candidate has not been published')
        receipt = package_sdk(repo, prefix, repo / 'out/native-validation' / run_id / 'summary.json',
                              cache_dir, checkpoint=lambda inputs: write_json(
                                  checkpoint, {**report, 'archive_inputs': inputs}))
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
        write_json(receipt_path, report)
        return {**report, 'cache_hit': False}
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(directory / 'build-report.json', report)
