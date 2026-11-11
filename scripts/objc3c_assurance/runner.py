"""Run canonical native owners without packaging an SDK for ordinary source CI."""
from __future__ import annotations

import json
import os
from pathlib import Path
import resource
import shutil
import subprocess
import sys
import time

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.build import build_foundation
from scripts.objc3c_toolchain.core import build_plans
from scripts.objc3c_toolchain.frontend_tests import qualify as frontend_tests
from scripts.objc3c_toolchain.package import verify_sdk
from scripts.objc3c_toolchain.qualify_installed import qualify_installed
from .evidence import SCHEMA, MAX_BYTES, artifact, digest, junit, lit, package_execution
from .plan import foundation_receipts, from_checkout, selected_suites, validate
from .execution import checked_command, execute_ctest, execute_suites
from .upstream import prepare_upstream_tools

# Existing Clang lit taxonomy, confined to touched compiler subsystems. Whole
# check-clang/check-llvm promotion awaits measured runner cost, not a new cron.
UPSTREAM = ('clang/test/AST', 'clang/test/SemaObjC', 'clang/test/SemaObjCXX',
            'clang/test/CodeGenObjC', 'clang/test/CodeGenObjCXX', 'clang/test/PCH',
            'clang/test/Modules', 'clang/test/Driver')


def _consumer_compile_jobs(repo: Path) -> int:
    """Size an installed-SDK consumer without requiring producer build state."""
    from scripts.objc3c_toolchain.host import resource_limits
    manifest = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())
    return resource_limits(manifest['build'])['compile_jobs']


def run(repo: Path, plan: dict, output: Path, *, shard: str | None = None,
        suite_ids: tuple[str, ...] | None = None, product_sdk: Path | None = None,
        package_release: Path | None = None) -> dict:
    validate(plan)
    if shard is not None and suite_ids is not None:
        raise ValueError('native execution cannot combine a canonical shard and an explicit suite partition')
    if suite_ids is None:
        suites = selected_suites(plan, shard)
    else:
        available = {row['id']: row for row in plan['suites']}
        if (not suite_ids or len(suite_ids) != len(set(suite_ids)) or
                any(name not in available for name in suite_ids)):
            raise ValueError('native suite partition is empty, duplicated or outside the exact plan')
        suites = [available[name] for name in suite_ids]
    if not suites:
        raise ValueError('native execution requires a nonempty suite selection')
    output = output.resolve()
    # Stale receipts must never be overwritten into a new apparent success.
    output.mkdir(parents=True, exist_ok=False)
    started = time.monotonic()
    report = {'schema': SCHEMA, 'status': 'running', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'suites': [], 'commands': [],
              'cache': {'restored_build_products': False, 'reason': plan['cache_policy']}}
    if shard is not None:
        report['shard'] = shard
    if suite_ids is not None:
        report['suite_partition'] = list(suite_ids)

    def save():
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        # Linux reports ru_maxrss in KiB; include descendant compiler/linker use.
        report['peak_child_rss_kib'] = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
        disk = shutil.disk_usage(repo)
        report['disk'] = {'total_bytes': disk.total, 'free_bytes': disk.free}
        write_json(output / 'result.json', report)

    # The workflow always uploads this root.  Persist it before resolving any
    # host or product path so an admission failure cannot erase its own
    # diagnostic and create a second, misleading artifact-upload failure.
    save()
    try:
        environment = {**os.environ, 'PYTHONDONTWRITEBYTECODE': '1', 'CCACHE_COMPILERCHECK': 'content'}
        bootstrap = Path(environment['OBJC3C_BOOTSTRAP_LLVM_ROOT']).resolve(strict=True)
        sdk = ((product_sdk.resolve(strict=True) if product_sdk is not None else
                repo / 'out/toolchains/objc3-development/stage-linux-x86_64'))
        environment['LD_LIBRARY_PATH'] = str(sdk / 'lib')
        report['bootstrap'] = {'clang_sha256': digest(bootstrap / 'bin/clang'),
                               'linker_sha256': digest(bootstrap / 'bin/ld.lld')}
        shared = {}
        prerequisites = set()
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        save()
        raise

    def product_identity(prefix: Path, release: dict | None = None) -> dict:
        receipt = json.loads((prefix / 'objc3-sdk.json').read_text())
        signatures = (receipt.get('source_build_id'), receipt.get('seal_sha256'),
                      receipt.get('target'))
        if (not all(isinstance(value, str) and value for value in signatures) or
                len(receipt['seal_sha256']) != 64 or
                any(char not in '0123456789abcdef' for char in receipt['seal_sha256'])):
            raise ValueError('product SDK receipt is missing its exact source, target or seal identity')
        result = {'source_build_id': receipt['source_build_id'],
                  'seal_sha256': receipt['seal_sha256'],
                  'host_environment': {'target_triple': receipt['target']}}
        if release is not None:
            result['archive_sha256'] = release['archive_sha256']
        return result

    try:
        if package_release is not None:
            package_release = package_release.resolve(strict=True)
            release = json.loads(package_release.read_text())
            installed_receipt = sdk / 'objc3-sdk.json'
            archive_sha256 = release.get('archive_sha256')
            if (release.get('status') != 'PASS' or not installed_receipt.is_file() or
                    not isinstance(archive_sha256, str) or len(archive_sha256) != 64 or
                    any(char not in '0123456789abcdef' for char in archive_sha256)):
                raise ValueError('restored product SDK or package receipt differs from the exact source build')
            installed = verify_sdk(sdk, release.get('seal_sha256'))
            release = {**release, 'installed': {**release.get('installed', {}), 'prefix': str(sdk)}}
            shared['release'] = release
            prerequisites.add('package')
            report['product'] = product_identity(sdk, release)
            if installed['source_build_id'] != report['product']['source_build_id']:
                raise ValueError('installed SDK inventory differs from the producer source build')
    except BaseException as error:
        report.update(status='FAIL', error=f'{type(error).__name__}: {error}')
        save()
        raise
    directory = None

    def command(label, argv, *, environment_override=None):
        try:
            checked_command(label, argv, repo=repo,
                            environment=environment if environment_override is None else environment_override,
                            directory=directory, records=report['commands'])
        finally:
            save()

    def profile_foundation(profile):
        if profile in shared and shared[profile].get('restored'):
            return Path(shared[profile]['prefix'])
        if profile not in shared:
            restored = environment.get('OBJC3C_ASAN_FOUNDATION_RECEIPT')
            if profile == 'compiler-asan-ubsan' and restored:
                from .instrumented_foundation import admit
                retained = Path(environment['OBJC3C_ASAN_FOUNDATION_EVIDENCE']).resolve(strict=True)
                evidence = directory / 'foundations' / profile
                shutil.copytree(retained, evidence)
                identity = Path(environment['OBJC3C_ASAN_FOUNDATION_IDENTITY'])
                shared[profile] = {'status': 'passed', 'restored': True,
                    'prefix': str(admit(repo, Path(restored), identity))}
                return Path(shared[profile]['prefix'])
            evidence = directory / 'foundations' / profile
            try:
                shared[profile] = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                    instrumentation_profile=profile, test_results=evidence)
            finally:
                receipt = (repo / 'out/toolchains/objc3-development' /
                    ('build-linux-x86_64-'+profile) / 'foundation-build-report.json')
                if receipt.is_file():
                    saved = json.loads(receipt.read_text())
                    write_json(evidence / 'foundation.json', saved)
                    for index, phase in enumerate(saved['phases']):
                        shutil.copyfile(phase['log'], evidence / ('phase-'+str(index)+'.log'))
        plan = build_plans(repo, host='linux-x86_64', instrumentation_profile=profile)['configure']
        return Path(plan['paths']['stage'])

    def profile_facade(profile):
        key = profile+'-facade'
        if key in shared:
            return shared[key]
        foundation = profile_foundation(profile)
        build = output / 'build' / key
        build.parent.mkdir(exist_ok=True)
        profile_environment = {**environment, 'LD_LIBRARY_PATH': str(foundation / 'lib')}
        command(key+'-configure', ['cmake', '-S', repo / 'native/objc3c', '-B', build, '-G', 'Ninja',
            f'-DOBJC3C_CLANG_FOUNDATION_ROOT={foundation}',
            f'-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE={profile}',
            *([f'-DOBJC3C_FOUNDATION_IDENTITY_FILE={environment["OBJC3C_ASAN_FOUNDATION_IDENTITY"]}']
              if profile == 'compiler-asan-ubsan' and environment.get('OBJC3C_ASAN_FOUNDATION_IDENTITY') else []),
            '-DOBJC3C_BUILD_SCAN_DEPS=OFF', '-DBUILD_TESTING=OFF',
            f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}', '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', f'-DPython3_EXECUTABLE={sys.executable}'],
            environment_override=profile_environment)
        command(key+'-build', ['cmake', '--build', build, '--target', 'objc3c', 'objc3c_frontend_v2',
                               '--parallel', '2'], environment_override=profile_environment)
        shared[key] = build
        return build

    def execute(suite, suite_directory):
        nonlocal directory
        directory = suite_directory
        name = suite['id']
        if name == 'foundation':
            try:
                foundation = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                                              test_results=directory)
            finally:
                # A failing compile must retain the actual compiler diagnostic,
                # even though the canonical builder raises before returning.
                receipt = repo / 'out/toolchains/objc3-development/build-linux-x86_64/foundation-build-report.json'
                if receipt.is_file():
                    saved = json.loads(receipt.read_text())
                    write_json(directory / 'foundation.json', saved)
                    for index, phase in enumerate(saved['phases']):
                        shutil.copyfile(phase['log'], directory / ('phase-' + str(index) + '.log'))
            executions = []
            for index, phase in enumerate(foundation['phases']):
                if phase['phase'] == 'test':
                    result = Path(phase['test_results'])
                    executions.extend(lit(result) if result.suffix == '.json' else junit(result))
            if foundation['status'] != 'passed' or not executions:
                raise ValueError('foundation has no passing native execution evidence')
            report.setdefault('product', product_identity(sdk))
            return executions
        if name == 'runtime':
            try:
                command('runtime', [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain_runtime.build_linux',
                        '--repo', repo, '--compiler-root', sdk, '--fetch', '--report-file', directory / 'runtime.json'])
            finally:
                receipt = directory / 'runtime.json'
                if receipt.is_file():
                    saved = json.loads(receipt.read_text())
                    build = repo / 'out/toolchains/libobjc2-v2.3/build-linux-x86_64' / saved['build_key']
                    for log in build.glob('step-*.log'):
                        shutil.copyfile(log, directory / log.name)
            runtime = json.loads((directory / 'runtime.json').read_text())
            if runtime['status'] != 'passed':
                raise ValueError('native runtime build did not pass')
            build = repo / 'out/native-assurance-runtime'
            root = repo / 'out/toolchains/libobjc2-v2.3'
            command('configure', ['cmake', '-S', repo / 'tests/clang_foundation/native_runtime',
                '-B', build, '-G', 'Ninja', *[f'-DCMAKE_{language}_COMPILER={sdk / "bin" / compiler}'
                    for language, compiler in (('C','clang'), ('CXX','clang++'), ('OBJC','clang'), ('OBJCXX','clang++'))],
                f'-DOBJC3_LIBOBJC2_SOURCE_DIR={root / "src"}',
                f'-DOBJC3_LIBOBJC2_BUILD_DIR={runtime["stage"]}',
                f'-DOBJC3_ROBIN_MAP_SOURCE_DIR={runtime["robin_map_source"]}', '-DBUILD_TESTING=ON'])
            command('build', ['cmake', '--build', build, '--parallel', '2'])
            tests = execute_ctest(build, 'runtime-ctest', directory, command)
            return tests
        if name == 'frontend':
            build = repo / 'out/toolchains/objc3-facade/build-linux-x86_64'
            command('configure', ['cmake', '-S', repo / 'native/objc3c', '-B', build, '-G', 'Ninja',
                f'-DOBJC3C_CLANG_FOUNDATION_ROOT={sdk}', f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}',
                f'-DPython3_EXECUTABLE={sys.executable}', '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld',
                '-DOBJC3C_BUILD_SCAN_DEPS=OFF', '-DBUILD_TESTING=ON'])
            jobs = _consumer_compile_jobs(repo)
            command('build', ['cmake', '--build', build, '--target', 'objc3c', '--parallel', str(jobs)])
            tests = frontend_tests(build, directory / 'frontend-tests', command, jobs=jobs)
            for row in tests['tests']:
                junit(directory / ('frontend-' + row['name'] + '-ctest.xml'), expected={row['name']})
            return tests['tests']
        if name == 'installed':
            result = qualify_installed(repo, sdk, directory / 'installed-tests')
            if result['status'] != 'PASS' or not result['commands']:
                raise ValueError('installed native source witnesses did not execute')
            return result['commands']
        if name == 'sanitizers':
            resource_root = Path(subprocess.check_output([bootstrap / 'bin/clang', '-print-resource-dir'], text=True).strip())
            command('executor-locks', [sys.executable, '-B', '-m',
                'scripts.objc3c_toolchain.qualify_executor_locks', '--repo', repo,
                '--compiler', sdk / 'bin/clang++', '--output', directory / 'executor-locks'])
            command('structured', [sys.executable, '-B', '-m',
                'scripts.objc3c_toolchain.qualify_structured_sanitizers', '--repo', repo,
                '--compiler-root', sdk, '--resource-root', resource_root, '--output-dir', directory / 'structured'])
            result = json.loads((directory / 'structured/qualification.json').read_text())
            if result['status'] != 'PASS' or len(result['executions']) != 4:
                raise ValueError('actual ASan/UBSan and TSan O0/O2 executions are incomplete')
            return result['executions']
        if name == 'upstream':
            foundation = repo / 'out/toolchains/objc3-development'
            # Test prerequisites stay outside the installed SDK distribution.
            tools = prepare_upstream_tools(repo, command)
            selected = [foundation / 'src' / name for name in UPSTREAM]
            write_json(directory / 'selection.json', {'selected': list(UPSTREAM),
                'tools': tools,
                'reason': 'unchanged-language regressions across AST/Sema/CodeGen/serialization/driver',
                'excluded': {'check-clang/check-llvm': 'whole-suite promotion requires measured runner cost; #8283'}})
            result_file = directory / 'lit.json'
            command('upstream-lit', [sys.executable, '-B', foundation / 'build-linux-x86_64/bin/llvm-lit',
                '-sv', '--show-unsupported', '-j', '2', '--output', result_file, *selected])
            return lit(result_file)
        if name == 'swift-consumer':
            from scripts.objc3c_toolchain.swift_consumer import prepare_compiler_prefix, qualify
            from scripts.objc3c_toolchain.swift_toolchain import install
            root = Path(environment['OBJC3C_CLOUD_SETUP_ROOT']) / 'swift-6.4.2'
            install(repo, root)
            prefix = directory / 'build/compiler-prefix'
            prepare_compiler_prefix(sdk, prefix, source_build_id=report['product']['source_build_id'],
                                    target=report['product']['host_environment']['target_triple'])
            result = qualify(repo, directory / 'consumer', sdk=prefix, swift_root=root,
                             symbolizer=bootstrap / 'bin/llvm-symbolizer')
            return result['cases']
        if name == 'semantic-workloads':
            from scripts.objc3c_toolchain.semantic_benchmarks import prepare_prefix, measure
            from scripts.objc3c_toolchain.semantic_import_benchmarks import measure as measure_imports
            prefix = directory / 'build/compiler-prefix'
            prepare_prefix(sdk, prefix, source_build_id=report['product']['source_build_id'],
                           target=report['product']['host_environment']['target_triple'], imports=True)
            result = measure(repo, directory / 'measurement', sdk=prefix, repetitions=3, extended_runtime=True)
            imports = measure_imports(repo, directory / 'imports', sdk=prefix, repetitions=3,
                                      scales=(32,), source_graph=True)
            # Correctness/completeness block the native gate. Timings have no
            # calibrated baseline here and cannot claim a performance pass.
            return result['summaries'] + imports['summaries']
        if name == 'package':
            from scripts.objc3c_toolchain.linux_sdk import build_sdk
            from scripts.objc3c_toolchain.semantic_build_costs import bounded_installation_receipt
            from .package_evidence import preserve, snapshot
            before = snapshot(repo)
            try:
                result = build_sdk(repo, fetch=True)
            finally:
                preserve(repo, directory / 'sdk-diagnostics', before)
            write_json(directory / 'release.json', result)
            if result['status'] != 'PASS':
                raise ValueError('packaging qualification failed')
            write_json(directory / 'installation-cost.json', bounded_installation_receipt(result))
            shared['release'] = result
            report['product'] = product_identity(Path(result['installed']['prefix']), result)
            return package_execution(result)
        if name == 'sanitizer-products':
            from scripts.objc3c_toolchain.sanitizer_products import qualify
            release = shared.get('release')
            if not release:
                raise ValueError('sanitizer product suite requires the selected package suite')
            compiler = profile_foundation('compiler-asan-ubsan')
            facade = profile_facade('compiler-asan-ubsan') / 'src/clang_foundation/objc3c'
            resource_root = Path(subprocess.check_output(
                [bootstrap / 'bin/clang', '-print-resource-dir'], text=True).strip())
            result = qualify(repo, Path(release['installed']['prefix']), compiler, facade,
                             resource_root, directory / 'campaign', jobs=2)
            if result['status'] != 'PASS' or len(result['owners']) != 13:
                raise ValueError('complete sanitizer product campaign did not pass')
            return sorted(result['owners'], key=lambda row: row['name'])
        if name == 'runtime-schedules':
            from scripts.objc3c_toolchain.runtime_schedules import qualify
            compiler = profile_foundation('compiler-asan-ubsan')
            resource_root = Path(subprocess.check_output(
                [bootstrap / 'bin/clang', '-print-resource-dir'], text=True).strip())
            result = qualify(repo, compiler, resource_root, directory / 'campaign')
            if result['status'] != 'PASS':
                raise ValueError('generated runtime schedule qualification did not pass')
            return result['cases'] + result['scenario_cases'] + result['controls']
        if name == 'frontend-fuzz':
            from scripts.objc3c_toolchain.codec_fuzz import qualify
            facade = profile_facade('compiler-fuzz-asan-ubsan')
            result = qualify(repo, directory / 'campaign', facade_build=facade, seconds=15, jobs=2)
            if result['status'] != 'PASS':
                raise ValueError('frontend/paired-codec fuzz qualification did not pass')
            return result['campaigns'] + result['controls']
        if name == 'source-coverage':
            # Fail fast on native profile ownership/merging before the expensive compiler build.
            merge_results = directory / 'profile-merge-regressions.xml'
            command('profile-merge-regressions', [sys.executable, '-B', '-m', 'pytest', '-q',
                repo / 'tests/tooling/test_source_coverage_profile_merging.py',
                '--junitxml='+str(merge_results)])
            if len(junit(merge_results)) != 4:
                raise ValueError('native profile merge regression inventory is incomplete')
            profile_foundation('compiler-source-coverage')
            from scripts.objc3c_toolchain.coverage_controls import qualify as controls
            from scripts.objc3c_toolchain.source_coverage import qualify as first_party
            from scripts.objc3c_toolchain.compiler_source_coverage import qualify as compiler_coverage
            control = controls(repo, directory / 'controls')
            native = first_party(repo, directory / 'first-party', jobs=2)
            compiler = compiler_coverage(repo, directory / 'compiler', jobs=2)
            if any(result['status'] != 'PASS' for result in (control, native, compiler)):
                raise ValueError('complete source coverage qualification did not pass')
            return ([{'name': 'coverage-controls', 'status': 'PASS', 'controls': len(control['controls'])},
                     {'name': 'native-profile-merge-controls', 'status': 'PASS', 'tests': 4}] +
                    [{'name': row['component']+'-'+row['name'], 'status': 'PASS'} for row in native['suites']] +
                    [{'name': row['name'], 'status': 'PASS'} for row in compiler['suites']])
        raise ValueError('unimplemented native suite: ' + name)

    try:
        save()
        execute_suites(suites, output=output, report=report, execute=execute, save=save,
                       prerequisites=prerequisites)
        if package_release is not None:
            verified = verify_sdk(sdk, report['product']['seal_sha256'])
            if verified['source_build_id'] != report['product']['source_build_id']:
                raise ValueError('product consumer changed the sealed SDK source identity')
        restored = environment.get('OBJC3C_ASAN_FOUNDATION_RECEIPT')
        if restored:
            from .instrumented_foundation import admit
            admit(repo, Path(restored), Path(environment['OBJC3C_ASAN_FOUNDATION_IDENTITY']))
        if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
            raise ValueError('source revision or selection changed during native execution')
        from scripts.objc3c_toolchain.semantic_build_costs import snapshot
        foundations = foundation_receipts(output, suites)
        installations = ([output / 'package/installation-cost.json']
                         if (output / 'package/installation-cost.json').is_file() else [])
        if foundations or installations:
            costs = snapshot(foundations=foundations, installations=installations)
            cost_file = output / 'operational-costs.json'
            write_json(cost_file, costs)
            report['operational_costs'] = artifact(output, cost_file)
        elif suite_ids is None:
            raise ValueError('canonical native shard omitted all operational measurement owners')
        size = sum(value['bytes'] for row in report['suites'] for value in row['artifacts'])
        size += report.get('operational_costs', {}).get('bytes', 0)
        if size > MAX_BYTES:
            raise ValueError('native evidence exceeds the 256 MiB artifact budget')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        save()
    return report
