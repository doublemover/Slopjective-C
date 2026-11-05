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
from scripts.objc3c_toolchain.qualify_installed import qualify_installed
from .evidence import SCHEMA, MAX_BYTES, artifact, digest, junit, lit, native_files
from .plan import from_checkout, validate
from .execution import checked_command, execute_ctest

# Existing Clang lit taxonomy, confined to touched compiler subsystems. Whole
# check-clang/check-llvm promotion awaits measured runner cost, not a new cron.
UPSTREAM = ('clang/test/AST', 'clang/test/SemaObjC', 'clang/test/SemaObjCXX',
            'clang/test/CodeGenObjC', 'clang/test/CodeGenObjCXX', 'clang/test/PCH',
            'clang/test/Modules', 'clang/test/Driver')


def run(repo: Path, plan: dict, output: Path) -> dict:
    validate(plan)
    if not plan['suites']:
        raise ValueError('native execution requires a nonempty suite selection')
    output = output.resolve()
    # Stale receipts must never be overwritten into a new apparent success.
    output.mkdir(parents=True, exist_ok=False)
    started = time.monotonic()
    report = {'schema': SCHEMA, 'status': 'running', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'suites': [], 'commands': [],
              'cache': {'restored_build_products': False, 'reason': plan['cache_policy']}}
    environment = {**os.environ, 'PYTHONDONTWRITEBYTECODE': '1', 'CCACHE_COMPILERCHECK': 'content'}
    bootstrap = Path(environment['OBJC3C_BOOTSTRAP_LLVM_ROOT']).resolve(strict=True)
    sdk = repo / 'out/toolchains/objc3-development/stage-linux-x86_64'
    environment['LD_LIBRARY_PATH'] = str(sdk / 'lib')
    report['bootstrap'] = {'clang_sha256': digest(bootstrap / 'bin/clang'),
                           'linker_sha256': digest(bootstrap / 'bin/ld.lld')}
    shared = {}
    directory = None

    def save():
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        # Linux reports ru_maxrss in KiB; include descendant compiler/linker use.
        report['peak_child_rss_kib'] = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
        disk = shutil.disk_usage(repo)
        report['disk'] = {'total_bytes': disk.total, 'free_bytes': disk.free}
        write_json(output / 'result.json', report)

    def command(label, argv, *, environment_override=None):
        try:
            checked_command(label, argv, repo=repo,
                            environment=environment if environment_override is None else environment_override,
                            directory=directory, records=report['commands'])
        finally:
            save()

    def profile_foundation(profile):
        if profile not in shared:
            shared[profile] = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                                               instrumentation_profile=profile)
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
            f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}', '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', f'-DPython3_EXECUTABLE={sys.executable}'],
            environment_override=profile_environment)
        command(key+'-build', ['cmake', '--build', build, '--target', 'objc3c', 'objc3c_frontend_v2',
                               '--parallel', '2'], environment_override=profile_environment)
        shared[key] = build
        return build

    def execute(suite):
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
            configure = build_plans(repo, host='linux-x86_64')['configure']
            report['product'] = {key: configure[key] for key in
                                 ('source_build_id', 'source_tree', 'build_config_digest', 'host_environment')}
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
            command('install', ['cmake', '--install', build, '--prefix', sdk])
            return tests
        if name == 'frontend':
            build = repo / 'out/toolchains/objc3-facade/build-linux-x86_64'
            command('configure', ['cmake', '-S', repo / 'native/objc3c', '-B', build, '-G', 'Ninja',
                f'-DOBJC3C_CLANG_FOUNDATION_ROOT={sdk}', f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}',
                f'-DPython3_EXECUTABLE={sys.executable}', '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', '-DBUILD_TESTING=ON'])
            jobs = build_plans(repo, host='linux-x86_64')['configure']['host_environment']['resource_limits']['compile_jobs']
            command('build', ['cmake', '--build', build, '--target', 'objc3c', 'clang-scan-deps', '--parallel', str(jobs)])
            tests = frontend_tests(build, directory / 'frontend-tests', command, jobs=jobs)
            for row in tests['tests']:
                junit(directory / ('frontend-' + row['name'] + '-ctest.xml'), expected={row['name']})
            command('install', ['cmake', '--install', build, '--prefix', sdk])
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
            selected = [foundation / 'src' / name for name in UPSTREAM]
            write_json(directory / 'selection.json', {'selected': list(UPSTREAM),
                'reason': 'unchanged-language regressions across AST/Sema/CodeGen/serialization/driver',
                'excluded': {'check-clang/check-llvm': 'whole-suite promotion requires measured runner cost; #8283'}})
            result_file = directory / 'lit.json'
            command('upstream-lit', [sys.executable, '-B', foundation / 'build-linux-x86_64/bin/llvm-lit',
                '-sv', '--show-unsupported', '-j', '2', '--output', result_file, *selected])
            return lit(result_file)
        if name == 'swift-consumer':
            from scripts.objc3c_toolchain.swift_consumer import prepare_compiler_prefix, qualify
            from scripts.objc3c_toolchain.swift_toolchain import install
            root = Path(environment['OBJC3C_CLOUD_SETUP_ROOT']) / 'swift-6.4.1'
            install(repo, root)
            prefix = directory / 'build/compiler-prefix'
            prepare_compiler_prefix(sdk, prefix, source_build_id=report['product']['source_build_id'],
                                    target=report['product']['host_environment']['target_triple'])
            result = qualify(repo, directory / 'consumer', sdk=prefix, swift_root=root,
                             symbolizer=bootstrap / 'bin/llvm-symbolizer')
            return result['cases']
        if name == 'semantic-workloads':
            from scripts.objc3c_toolchain.semantic_benchmarks import prepare_prefix, measure
            prefix = directory / 'build/compiler-prefix'
            prepare_prefix(sdk, prefix, source_build_id=report['product']['source_build_id'],
                           target=report['product']['host_environment']['target_triple'])
            result = measure(repo, directory / 'measurement', sdk=prefix, repetitions=3)
            # Correctness/completeness block the native gate. Timings have no
            # calibrated baseline here and cannot claim a performance pass.
            return result['summaries']
        if name == 'package':
            from scripts.objc3c_toolchain.linux_sdk import build_sdk
            from .package_evidence import preserve, snapshot
            before = snapshot(repo)
            try:
                result = build_sdk(repo, fetch=True)
            finally:
                preserve(repo, directory / 'sdk-diagnostics', before)
            write_json(directory / 'release.json', result)
            if result['status'] != 'PASS':
                raise ValueError('packaging qualification failed')
            # Exercise the shipped artifact through the isolated installer/relocator,
            # in this host distribution. The two-distribution matrix remains release CI.
            command('archive-install', ['sh', repo / 'scripts/qualify_linux_archive.sh',
                result['archive'], result['archive_sha256'], directory / 'isolated-install'])
            shared['release'] = result
            return [{'archive_sha256': result['archive_sha256'], 'status': 'PASS'}]
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
            profile_foundation('compiler-source-coverage')
            from scripts.objc3c_toolchain.coverage_controls import qualify as controls
            from scripts.objc3c_toolchain.source_coverage import qualify as first_party
            from scripts.objc3c_toolchain.compiler_source_coverage import qualify as compiler_coverage
            control = controls(repo, directory / 'controls')
            native = first_party(repo, directory / 'first-party', jobs=2)
            compiler = compiler_coverage(repo, directory / 'compiler', jobs=2)
            if any(result['status'] != 'PASS' for result in (control, native, compiler)):
                raise ValueError('complete source coverage qualification did not pass')
            return ([{'name': 'coverage-controls', 'status': 'PASS', 'controls': len(control['controls'])}] +
                    [{'name': row['component']+'-'+row['name'], 'status': 'PASS'} for row in native['suites']] +
                    [{'name': row['name'], 'status': 'PASS'} for row in compiler['suites']])
        raise ValueError('unimplemented native suite: ' + name)

    try:
        save()
        for suite in plan['suites']:
            directory = output / suite['id']
            directory.mkdir()
            row = {'id': suite['id'], 'status': 'running', 'executions': [], 'artifacts': []}
            report['suites'].append(row)
            save()
            try:
                row['executions'] = execute(suite)
                # Upload logs/results only, never the generated compilers, SDKs,
                # runtime archives or complete CMake build trees.
                row['artifacts'] = [artifact(output, path) for path in native_files(directory)]
                if not row['executions'] or not row['artifacts']:
                    raise ValueError('native suite produced no execution or artifact inventory')
                row['status'] = 'PASS'
            except BaseException as error:
                # Nested owners can fail before returning an execution list.
                # Their actual diagnostics must still join the durable upload.
                row['artifacts'] = [artifact(output, path) for path in native_files(directory)]
                row.update(status='FAIL', error=str(error))
                raise
            finally:
                save()
        if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
            raise ValueError('source revision or selection changed during native execution')
        size = sum(value['bytes'] for row in report['suites'] for value in row['artifacts'])
        if size > MAX_BYTES:
            raise ValueError('native evidence exceeds the 256 MiB artifact budget')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        save()
    return report
