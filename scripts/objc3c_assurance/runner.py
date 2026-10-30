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
    directory = None

    def save():
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        # Linux reports ru_maxrss in KiB; include descendant compiler/linker use.
        report['peak_child_rss_kib'] = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
        disk = shutil.disk_usage(repo)
        report['disk'] = {'total_bytes': disk.total, 'free_bytes': disk.free}
        write_json(output / 'result.json', report)

    def command(label, argv):
        try:
            checked_command(label, argv, repo=repo, environment=environment,
                            directory=directory, records=report['commands'])
        finally:
            save()

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
        if name == 'package':
            from scripts.objc3c_toolchain.linux_sdk import build_sdk
            result = build_sdk(repo, fetch=True)
            write_json(directory / 'release.json', result)
            if result['status'] != 'PASS':
                raise ValueError('packaging qualification failed')
            # Exercise the shipped artifact through the isolated installer/relocator,
            # in this host distribution. The two-distribution matrix remains release CI.
            command('archive-install', ['sh', repo / 'scripts/qualify_linux_archive.sh',
                result['archive'], result['archive_sha256'], directory / 'isolated-install'])
            return [{'archive_sha256': result['archive_sha256'], 'status': 'PASS'}]
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
