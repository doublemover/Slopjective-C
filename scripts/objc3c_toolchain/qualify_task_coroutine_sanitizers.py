"""Instrument native coroutine frames, await handoff and runtime together."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

from .binary_packages import digest, disjoint, write_json
from .sanitizer_policy import SanitizerQualification, require_clean_diagnostic


def configurations():
    return [(sanitizer, optimization, ownership)
            for sanitizer in ('address,undefined', 'thread')
            for optimization in ('O0', 'O2') for ownership in ('none', 'arc', 'mrc')]


def qualify(repo: Path, compiler_root: Path, resource_root: Path, output: Path,
            *, objc_runtime: Path | None = None) -> dict:
    repo, compiler_root = repo.resolve(strict=True), compiler_root.resolve(strict=True)
    resource_root, output = resource_root.resolve(strict=True), output.resolve()
    objc_runtime = objc_runtime or compiler_root / 'lib/libobjc.so'
    disjoint(output, compiler_root)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    sources = [repo / 'native/objc3c/src/runtime' / name for name in (
        'services/error_service.cpp', 'services/objc_task_error.cpp',
        'services/carrier_service.cpp',
        'concurrency/service_executor.cpp', 'concurrency/service_task.cpp', 'concurrency/service_async_join.cpp',
        'concurrency/service_group.cpp', 'concurrency/service_actor.cpp',
        'concurrency/service_public.cpp')]
    fixture = repo / 'tests/clang_foundation/task_coroutines'
    inputs = sources + sorted(fixture.glob('*'))
    inputs += sorted((repo / 'native/objc3c/src/runtime').rglob('*.h'))
    inputs += sorted((repo / 'native/objc3c/src/runtime/public').glob('*.hpp'))
    cases, commands = [], []
    policies = {}

    def run(label, argv, environment, *, cwd=None):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), env=environment, cwd=cwd, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=180)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': digest(log)})
        if result.returncode:
            raise ValueError('instrumented task coroutines failed: ' + label)
        require_clean_diagnostic(log.read_text(errors="replace"))

    try:
        for sanitizer, optimization, ownership in configurations():
            if sanitizer not in policies:
                policies[sanitizer] = SanitizerQualification(compiler_root / 'bin/clang++',
                    resource_root, output / 'detector-controls' / sanitizer.replace(',', '-'),
                    sanitizer, libraries=(objc_runtime,), additional_inputs=(Path(__file__),), base_environment=os.environ)
            policy = policies[sanitizer]
            policy.ensure_controls(optimization)
            with tempfile.TemporaryDirectory(prefix='objc3-task-sanitizers-') as temporary:
                work = Path(temporary)
                label = sanitizer.replace(',', '-') + '-' + optimization + '-' + ownership
                executable = work / label
                env = policy.environment
                flags = []
                if ownership != 'none':
                    flags = ['-x', 'objective-c++', '-fobjc-version=3',
                             '-fobjc-runtime=gnustep-2.2', '-fblocks',
                             '-fobjc-arc' if ownership == 'arc' else '-fno-objc-arc']
                common = [compiler_root / 'bin/clang++', '--no-default-config',
                    '-std=c++20', '-g', '-' + optimization, '-UNDEBUG', '-fno-omit-frame-pointer', '-fno-sanitize-recover=undefined',
                    '-fcoro-aligned-allocation',
                    '-fsanitize=' + sanitizer, '-resource-dir', resource_root, '-pthread',
                    '-I' + str(repo / 'native/objc3c/src'),
                    '-I' + str(repo / 'native/objc3c/src/runtime/public')]
                # Language options apply to the whole driver invocation. Keep
                # services in C++, and reuse their instrumented objects for the
                # provider/client and group consumer in this ownership mode.
                objects_dir = work / 'runtime-objects'
                objects_dir.mkdir()
                objects = [objects_dir / (source.stem + '.o') for source in sources]
                run(label + '-runtime-build', [*common, '-c', *sources], env,
                    cwd=objects_dir)
                run(label + '-build', [*common, '-fuse-ld=lld', *flags,
                    fixture / 'provider.cpp', fixture / 'client.cpp',
                    '-x', 'none', *objects, objc_runtime,
                    '-Wl,-rpath,' + str(objc_runtime.parent), '-o', executable], env)
                run(label + '-execute', [executable], env)
                group_executable = work / (label + '-group-await')
                run(label + '-group-await-build', [*common, '-fuse-ld=lld',
                    '-Wl,--wrap=objc3_task_group_scope_enter',
                    *flags, fixture / 'group_await.cpp',
                    '-x', 'none', *objects, objc_runtime,
                    '-Wl,-rpath,' + str(objc_runtime.parent), '-o', group_executable], env)
                run(label + '-group-await-execute', [group_executable], env)
                cases.append({'sanitizer': sanitizer, 'optimization': optimization, 'ownership': ownership,
                              'group_await_executable_sha256': digest(group_executable),
                              'executable_sha256': digest(executable)})
        report = {'schema': 'objc3c.task-coroutine-sanitizers.v1', 'status': 'PASS',
                  'compiler_root': str(compiler_root), 'compiler_sha256': digest(compiler_root / 'bin/clang'),
                  'sanitizer_resource_root': str(resource_root),
                  'objc_runtime_sha256': digest(objc_runtime),
                  'instrumentation': 'native coroutine provider/client, task/group await, quiescence, executor, group, actor, value/error services',
                  'executions': cases, 'commands': commands,
                  'sanitizer_policy': {mode: policy.finalize() for mode, policy in policies.items()},
                  'source_hashes': {str(path.relative_to(repo)): digest(path) for path in inputs},
                  'sanitizer_runtime_sha256': {str(path.relative_to(resource_root)): digest(path)
                      for path in sorted(resource_root.rglob('libclang_rt.*san*.a'))}}
    except Exception as error:
        write_json(receipt, {'schema': 'objc3c.task-coroutine-sanitizers.v1', 'status': 'FAIL',
                            'executions': cases, 'commands': commands, 'error': str(error)})
        raise
    write_json(receipt, report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--compiler-root', type=Path, required=True)
    parser.add_argument('--resource-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--objc-runtime', type=Path)
    args = parser.parse_args(argv)
    report = qualify(args.repo, args.compiler_root, args.resource_root, args.output_dir,
                     objc_runtime=args.objc_runtime)
    print(json.dumps({'status': report['status'], 'executions': len(report['executions'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
