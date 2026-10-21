"""Instrument native coroutine frames, await handoff and runtime together."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

from .binary_packages import digest, disjoint, write_json


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
        'concurrency/service_executor.cpp', 'concurrency/service_task.cpp',
        'concurrency/service_group.cpp', 'concurrency/service_actor.cpp',
        'concurrency/service_public.cpp')]
    fixture = repo / 'tests/clang_foundation/task_coroutines'
    inputs = sources + sorted(fixture.glob('*'))
    inputs += sorted((repo / 'native/objc3c/src/runtime').rglob('*.h'))
    inputs += sorted((repo / 'native/objc3c/src/runtime/public').glob('*.hpp'))
    cases, commands = [], []

    def run(label, argv, environment):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), env=environment, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=180)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': digest(log)})
        if result.returncode:
            raise ValueError('instrumented task coroutines failed: ' + label)

    try:
        for sanitizer, optimization, ownership in configurations():
            with tempfile.TemporaryDirectory(prefix='objc3-task-sanitizers-') as temporary:
                work = Path(temporary)
                label = sanitizer.replace(',', '-') + '-' + optimization + '-' + ownership
                executable = work / label
                env = {**os.environ, 'ASAN_OPTIONS': 'detect_leaks=1:halt_on_error=1',
                       'UBSAN_OPTIONS': 'halt_on_error=1:print_stacktrace=1',
                       'TSAN_OPTIONS': 'halt_on_error=1'}
                env['LD_LIBRARY_PATH'] = str(objc_runtime.parent)
                flags = []
                if ownership != 'none':
                    flags = ['-x', 'objective-c++', '-fobjc-runtime=gnustep-2.2', '-fblocks',
                             '-fobjc-arc' if ownership == 'arc' else '-fno-objc-arc']
                run(label + '-build', [compiler_root / 'bin/clang++', '--no-default-config',
                    '-std=c++20', '-g', '-' + optimization, '-UNDEBUG', '-fno-omit-frame-pointer',
                    '-fcoro-aligned-allocation',
                    '-fsanitize=' + sanitizer, '-resource-dir', resource_root, '-pthread', '-fuse-ld=lld',
                    '-I' + str(repo / 'native/objc3c/src'),
                    '-I' + str(repo / 'native/objc3c/src/runtime/public'),
                    *sources, *flags, fixture / 'provider.cpp', fixture / 'client.cpp',
                    '-x', 'none', objc_runtime, '-Wl,-rpath,' + str(objc_runtime.parent), '-o', executable], env)
                run(label + '-execute', [executable], env)
                cases.append({'sanitizer': sanitizer, 'optimization': optimization, 'ownership': ownership,
                              'executable_sha256': digest(executable)})
        report = {'schema': 'objc3c.task-coroutine-sanitizers.v1', 'status': 'PASS',
                  'compiler_root': str(compiler_root), 'compiler_sha256': digest(compiler_root / 'bin/clang'),
                  'sanitizer_resource_root': str(resource_root),
                  'objc_runtime_sha256': digest(objc_runtime),
                  'instrumentation': 'native coroutine provider/client, task await, executor, group, actor, value/error services',
                  'executions': cases, 'commands': commands,
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
