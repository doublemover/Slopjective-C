"""Plan and resource-admit a separate expensive-check compiler diagnostic pilot.

This is deliberately outside the product's foundation/profile identities. It
never restages or repoints the normal compiler, SDK or embedding consumers.
Resource rejection and timeout are explicit incomplete results, not qualification.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import time

from .binary_packages import digest, write_json
from .core import build_plans
from .llvm_ir_execution import command
from .llvm_tools import identity
from .sanitizer_policy import strict_environment


def tree_bytes(root: Path) -> int:
    # Do not follow symlinks or double-count hardlinked install aliases.
    inodes, total = set(), 0
    for path in root.rglob('*'):
        if path.is_file() and not path.is_symlink():
            stat = path.stat()
            key = stat.st_dev, stat.st_ino
            if key not in inodes:
                inodes.add(key)
                total += stat.st_size
    return total


def resource_decision(*, measured_build: int, measured_stage: int, free: int,
                      factor: float, reserve: int) -> dict:
    if min(measured_build, measured_stage, free, reserve) < 0 or factor < 1:
        raise ValueError('invalid expensive-check pilot resource measurements')
    required = int((measured_build+measured_stage)*factor)+reserve
    return {'status': 'admitted' if free >= required else 'resource-constrained',
            'measured_comparable_build_bytes': measured_build, 'measured_comparable_stage_bytes': measured_stage,
            'free_bytes': free, 'estimated_required_bytes': required,
            'headroom_factor': factor, 'reserve_bytes': reserve,
            'estimate_boundary': 'Measured comparable instrumented build, not an expensive-check build measurement'}


def plan(repo: Path) -> dict:
    preset = repo / 'toolchains/llvm/expensive-checks.json'
    document = json.loads(preset.read_text())
    if (document.get('schema') != 'objc3c.llvm-expensive-check-pilot.v1' or
            document.get('base_profile') != 'compiler-asan-ubsan' or
            document.get('cmake') != {'LLVM_ENABLE_EXPENSIVE_CHECKS': 'ON'} or
            document.get('targets') != ['clang', 'opt', 'llc', 'llvm-dis']):
        raise ValueError('unreviewed expensive-check pilot preset')
    base = build_plans(repo, host='linux-x86_64', instrumentation_profile=document['base_profile'])['configure']
    completed = json.loads((Path(base['paths']['build']) / 'foundation-build-report.json').read_text())
    if completed.get('status') != 'passed' or completed.get('source_identity') != base['source_build_id']:
        raise ValueError('resource estimate requires a completed matching comparable foundation')
    binding = {'source_build_id': base['source_build_id'], 'build_config_digest': base['build_config_digest'],
               'source_tree': base['source_tree'], 'host_environment': base['host_environment'],
               'preset_sha256': digest(preset), 'owner_sha256': digest(Path(__file__))}
    key = identity(binding)
    root = repo / 'out/toolchains' / ('objc3-expensive-checks-'+key[:16])
    build, stage = root / 'build', root / 'stage'
    old_build, old_stage = (Path(base['paths'][name]) for name in ('build', 'stage'))
    args = [arg.replace(str(old_build), str(build)).replace(str(old_stage), str(stage))
            for arg in base['command']]
    args = [('-DCLANG_OBJC3_CONFIG_DIGEST='+key if arg.startswith('-DCLANG_OBJC3_CONFIG_DIGEST=') else arg)
            for arg in args]
    args += ['-DLLVM_ENABLE_EXPENSIVE_CHECKS=ON']
    resources = resource_decision(measured_build=tree_bytes(old_build), measured_stage=tree_bytes(old_stage),
        free=shutil.disk_usage(repo).free, factor=document['disk_headroom_factor'], reserve=document['reserve_bytes'])
    return {'schema': 'objc3c.llvm-expensive-check-plan.v1', 'identity': key, 'binding': binding,
            'paths': {'source': base['paths']['source'], 'build': str(build), 'stage': str(stage)},
            'configure': args, 'targets': document['targets'], 'resource_admission': resources,
            'jobs': base['host_environment']['resource_limits']['compile_jobs'],
            'boundary': document['boundary'], 'qualified': False}


def pilot(repo: Path, output: Path, *, timeout=1800) -> dict:
    if output.exists() or not output.resolve().is_relative_to(repo / 'out'):
        raise ValueError('expensive-check pilot needs a fresh repository out evidence directory')
    started = time.monotonic()
    report = plan(repo)
    output.mkdir(parents=True)
    report['status'] = report['resource_admission']['status']
    report['commands'] = []
    try:
        if report['status'] != 'admitted':
            return report
        bootstrap = Path(report['binding']['host_environment']['bootstrap_root'])
        env = strict_environment(bootstrap / 'bin/clang++', bootstrap / 'bin/llvm-symbolizer')
        host = report['binding']['host_environment']
        build = Path(report['paths']['build'])
        saved_plan = build / 'expensive-check-plan.json'
        if build.exists() and (not saved_plan.is_file() or
                json.loads(saved_plan.read_text()).get('identity') != report['identity']):
            raise ValueError('existing expensive-check build lacks the same isolated plan')
        build.mkdir(parents=True, exist_ok=True)
        write_json(saved_plan, report)
        commands = [[host['cmake_path'], *report['configure'][1:]],
                    [host['cmake_path'], '--build', report['paths']['build'], '--target', *report['targets'],
                     '--parallel', str(report['jobs'])]]
        # The absolute Ninja path is passed to CMake; do not resolve an ambient
        # build tool while planning a separate compiler ABI configuration.
        commands[0].append('-DCMAKE_MAKE_PROGRAM='+host['ninja_path'])
        for index, argv in enumerate(commands):
            remaining = timeout-(time.monotonic()-started)
            if remaining <= 0:
                report['status'] = 'incomplete-timeout'
                return report
            row = command(argv, cwd=repo, environment=env, log=output / (str(index)+'.log'), timeout=remaining)
            report['commands'].append(row)
            if row['timed_out'] or row['exit_code']:
                report['status'] = 'incomplete-timeout' if row['timed_out'] else 'failed'
                return report
        report['status'] = 'built-unqualified'
        return report
    finally:
        report['seconds'] = round(time.monotonic()-started, 6)
        write_json(output / 'pilot.json', report)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--timeout', type=float, default=1800)
    args = parser.parse_args(argv)
    if args.timeout <= 0:
        parser.error('timeout must be positive')
    report = pilot(args.repo.resolve(strict=True), args.output, timeout=args.timeout)
    print(json.dumps({'status': report['status'], 'qualified': report['qualified'],
                      'resource_admission': report['resource_admission']}))
    return 0 if report['status'] in ('resource-constrained', 'built-unqualified') else 1


if __name__ == '__main__':
    raise SystemExit(main())
