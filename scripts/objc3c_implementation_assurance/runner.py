"""Build the exact fork and run implementation analysis outside the long native lane."""
from __future__ import annotations

import json
import os
from pathlib import Path
import shutil
import sys
import time

from scripts.objc3c_assurance.execution import checked_command
from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.build import build_foundation
from scripts.objc3c_toolchain.core import build_plans
from scripts.objc3c_toolchain.implementation_analysis import qualify
from scripts.objc3c_toolchain.sanitizer_policy import strict_environment

from .evidence import SCHEMA, MAX_BYTES, analysis_results, artifact, evidence_files
from .plan import validate

PROFILE = 'compiler-asan-ubsan'


def run(repo: Path, plan: dict, output: Path) -> dict:
    validate(plan)
    if not plan['required']:
        raise ValueError('unaffected implementation assurance must not execute')
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('implementation assurance requires fresh owned evidence')
    output.mkdir(parents=True)
    logs = output / 'build-logs'
    logs.mkdir()
    report = {'schema': SCHEMA, 'status': 'running', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'analysis_paths': plan['analysis_paths'],
              'commands': [], 'artifacts': [], 'temporary_builds_reclaimed': False}
    started = time.monotonic()
    configuration = None
    build_root = repo / 'out/implementation-assurance-builds'
    frontend_build = build_root / 'frontend'
    runtime_build = build_root / 'runtime'
    tool_build = build_root / 'tool'
    foundation_root = repo / 'out/toolchains/objc3-development'
    foundation_build = foundation_root / ('build-linux-x86_64-'+PROFILE)
    temporary = [build_root, foundation_root / 'src', foundation_build,
                 foundation_root / ('stage-linux-x86_64-'+PROFILE)]

    def save():
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        write_json(output / 'result.json', report)

    def command(name, argv, environment):
        try:
            checked_command(name, argv, repo=repo, environment=environment,
                            directory=logs, records=report['commands'])
        finally:
            save()

    def preserve_foundation():
        receipt = foundation_build / 'foundation-build-report.json'
        if not receipt.is_file():
            return
        directory = output / 'foundation'
        directory.mkdir(exist_ok=True)
        saved = json.loads(receipt.read_text())
        write_json(directory / 'foundation.json', saved)
        for index, phase in enumerate(saved.get('phases', [])):
            path = Path(phase.get('log', ''))
            if path.is_file():
                shutil.copyfile(path, directory / ('phase-'+str(index)+'.log'))

    try:
        save()
        foundation = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                                      instrumentation_profile=PROFILE,
                                      test_results=output / 'foundation')
        configuration = build_plans(repo, host='linux-x86_64',
                                    instrumentation_profile=PROFILE)['configure']
        preserve_foundation()
        if foundation.get('status') != 'passed':
            raise ValueError('instrumented foundation did not pass')
        stage = Path(configuration['paths']['stage']).resolve(strict=True)
        bootstrap = Path(configuration['host_environment']['bootstrap_root']).resolve(strict=True)
        cmake = configuration['host_environment']['cmake_path']
        jobs = configuration['host_environment']['resource_limits']['compile_jobs']
        # Configuration/build processes can load the instrumented fork. Keep
        # sanitizer behavior fail-closed while retaining the pinned CMake/Ninja
        # directories installed by the clean-host bootstrap.
        environment = strict_environment(bootstrap / 'bin/clang++',
            bootstrap / 'bin/llvm-symbolizer', library_dirs=(stage / 'lib',), base=os.environ)
        environment['PATH'] = os.pathsep.join(dict.fromkeys((
            str(Path(cmake).parent), str(Path(configuration['host_environment']['ninja_path']).parent),
            environment['PATH'])))
        environment['PYTHONDONTWRITEBYTECODE'] = '1'
        environment['CCACHE_COMPILERCHECK'] = 'content'
        frontend_build.parent.mkdir(parents=True, exist_ok=True)
        command('frontend-configure', [cmake, '-S', repo / 'native/objc3c', '-B', frontend_build,
            '-G', 'Ninja', f'-DOBJC3C_CLANG_FOUNDATION_ROOT={stage}',
            f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}',
            f'-DPython3_EXECUTABLE={sys.executable}', '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld',
            '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_TESTING=ON'], environment)
        command('frontend-build', [cmake, '--build', frontend_build, '--target', 'objc3c',
            'clang-scan-deps', '--parallel', str(jobs)], environment)
        command('runtime-configure', [cmake, '-S', repo / 'native/objc3c/src/runtime',
            '-B', runtime_build, '-G', 'Ninja',
            f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', '-DCMAKE_BUILD_TYPE=Release',
            '-DBUILD_TESTING=ON'], environment)
        command('runtime-build', [cmake, '--build', runtime_build,
            '--target', 'objc3_runtime_services', '--parallel', str(jobs)], environment)
        qualification = qualify(repo, output / 'analysis', frontend_build=frontend_build,
            runtime_build=runtime_build, tool_build=tool_build,
            changed=[repo / path for path in plan['analysis_paths']], jobs=min(4, max(1, jobs)))
        if qualification.get('status') != 'PASS':
            raise ValueError('fork-matched implementation analysis did not pass')
        results = analysis_results(output)
        report.update(status='PASS', analyzed_variants=len(results),
                      inventory_variants=qualification['inventory_variants'],
                      source_build_id=qualification['source_build_id'],
                      build_config_digest=qualification['build_config_digest'])
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        preserve_foundation()
        roots = list(temporary)
        if configuration is not None:
            roots += [Path(configuration['paths'][key]) for key in ('build', 'stage', 'source')]
        for path in roots:
            path = path.resolve()
            if path.is_relative_to(repo / 'out'):
                shutil.rmtree(path, ignore_errors=True)
        report['temporary_builds_reclaimed'] = not any(path.exists() for path in roots)
        report['artifacts'] = [artifact(output, path) for path in evidence_files(output)]
        size = sum(row['bytes'] for row in report['artifacts'])
        if report.get('status') == 'PASS' and (not report['temporary_builds_reclaimed'] or size > MAX_BYTES):
            report.update(status='FAIL', error='temporary builds remain or evidence exceeds its artifact budget')
        save()
    if report['status'] != 'PASS':
        raise ValueError(report.get('error', 'implementation assurance did not pass'))
    return report
