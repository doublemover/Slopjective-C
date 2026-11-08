"""Build the exact fork and run implementation analysis outside the long native lane."""
from __future__ import annotations

import json
import os
from pathlib import Path
import shutil
import sys
import time

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.build import _build_foundation, foundation_lock
from scripts.objc3c_toolchain.core import build_plans
from scripts.objc3c_toolchain.implementation_analysis import qualify
from scripts.objc3c_toolchain.llvm_ir_execution import command as execute_command
from scripts.objc3c_toolchain.sanitizer_policy import require_clean_diagnostic, strict_environment

from .evidence import SCHEMA, MAX_BYTES, analysis_results, artifact, evidence_files
from .plan import from_checkout, validate

PROFILE = 'compiler-asan-ubsan'


def owned_path(repo: Path, path: Path) -> Path:
    """Reject linked output components before creating or deleting anything."""
    path = path.absolute()
    if '..' in path.parts or not path.is_relative_to(repo / 'out') or path == repo / 'out':
        raise ValueError('implementation assurance output escaped repository out')
    for component in (path, *path.parents):
        if component == repo:
            break
        if component.is_symlink():
            raise ValueError('implementation assurance output contains a symlink: '+str(component))
    return path


def run(repo: Path, plan: dict, output: Path, *, partition_index=0) -> dict:
    validate(plan)
    from scripts.objc3c_toolchain.analysis_partition import partition_spec
    partition = partition_spec(partition_index, plan['analysis_partitions'])
    if not plan['required']:
        raise ValueError('unaffected implementation assurance must not execute')
    repo = repo.resolve(strict=True)
    output = owned_path(repo, output)
    # Keep the canonical lock through database preparation, analysis and cleanup.
    # The locked builder is used below so it does not reacquire this same lock.
    owned_path(repo, repo / 'out/toolchains/objc3-development/.foundation-build.lock')
    with foundation_lock(repo):
        return _run_locked(repo, plan, output, partition)


def _run_locked(repo: Path, plan: dict, output: Path, partition: dict) -> dict:
    if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
        raise ValueError('implementation assurance source changed before execution')
    if output.exists():
        raise ValueError('implementation assurance requires fresh owned evidence')
    build_root = repo / 'out/implementation-assurance-builds'
    frontend_build = build_root / 'frontend'
    runtime_build = build_root / 'runtime'
    tool_build = build_root / 'tool'
    foundation_root = repo / 'out/toolchains/objc3-development'
    foundation_build = foundation_root / ('build-linux-x86_64-'+PROFILE)
    temporary = [build_root, foundation_root / 'src', foundation_build,
                 foundation_root / ('stage-linux-x86_64-'+PROFILE)]
    for path in temporary:
        owned_path(repo, path)
        if path.exists():
            raise ValueError('implementation assurance requires a clean build root; preserving '+str(path))
        if output.is_relative_to(path) or path.is_relative_to(output):
            raise ValueError('implementation assurance evidence overlaps temporary build roots')
    output.mkdir(parents=True)
    logs = output / 'build-logs'
    logs.mkdir()
    report = {'schema': SCHEMA, 'status': 'running', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'analysis_paths': plan['analysis_paths'],
              'evidence_root': str(output), 'partition': partition,
              'commands': [], 'artifacts': [], 'temporary_builds_reclaimed': False}
    started = time.monotonic()

    def save():
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        write_json(output / 'result.json', report)

    def command(name, argv, environment):
        try:
            print('implementation assurance '+name+': '+str(logs / (name+'.log')), flush=True)
            row = execute_command(argv, cwd=repo, environment=environment,
                                  log=logs / (name+'.log'), timeout=1800)
            row['name'] = name
            report['commands'].append(row)
            if row['timed_out'] or row['exit_code'] != 0:
                raise ValueError('implementation assurance command failed: '+name)
            require_clean_diagnostic(Path(row['log']).read_text(errors='replace'))
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
                if path.is_symlink() or not path.resolve().is_relative_to(foundation_build):
                    raise ValueError('foundation diagnostic escaped its owned build')
                shutil.copyfile(path, directory / ('phase-'+str(index)+'.log'))

    try:
        save()
        foundation = _build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
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
            f'-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE={PROFILE}', '-DOBJC3C_LINK_CLANG_CPP=ON',
            f'-DCMAKE_CXX_COMPILER={bootstrap / "bin/clang++"}',
            f'-DPython3_EXECUTABLE={sys.executable}', '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld',
            '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_TESTING=OFF'], environment)
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
            changed=[repo / path for path in plan['analysis_paths']], jobs=min(4, max(1, jobs)),
            partition_index=partition['index'], partition_count=partition['count'])
        if qualification.get('status') != 'PASS':
            raise ValueError('fork-matched implementation analysis did not pass')
        results = analysis_results(output)
        if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
            raise ValueError('implementation assurance source changed during execution')
        report.update(status='PASS', analyzed_variants=len(results),
                      inventory_variants=qualification['inventory_variants'],
                      source_build_id=qualification['source_build_id'],
                      build_config_digest=qualification['build_config_digest'])
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        # Diagnostic retention failure must not skip cleanup or erase the original
        # error. Delete only the exact roots this attempt admitted as absent.
        try:
            preserve_foundation()
        except (OSError, ValueError) as error:
            report.update(status='FAIL', diagnostic_error=str(error))
        cleanup_errors = []
        for path in temporary:
            try:
                owned_path(repo, path)
                if path.exists():
                    shutil.rmtree(path)
            except (OSError, ValueError) as error:
                cleanup_errors.append(str(error))
        report['temporary_builds_reclaimed'] = not cleanup_errors and not any(
            path.exists() or path.is_symlink() for path in temporary)
        if cleanup_errors:
            report.update(status='FAIL', cleanup_errors=cleanup_errors)
        report['artifacts'] = [artifact(output, path) for path in evidence_files(output)]
        size = sum(row['bytes'] for row in report['artifacts'])
        if report.get('status') == 'PASS' and (not report['temporary_builds_reclaimed'] or size > MAX_BYTES):
            report.update(status='FAIL', error='temporary builds remain or evidence exceeds its artifact budget')
        save()
    if report['status'] != 'PASS':
        raise ValueError(report.get('error', 'implementation assurance did not pass'))
    return report
