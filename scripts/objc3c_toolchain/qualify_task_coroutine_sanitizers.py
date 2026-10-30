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
from .runtime_instrumentation import RuntimeProfiles


def configurations():
    return [(sanitizer, optimization, ownership)
            for sanitizer in ('address,undefined', 'thread')
            for optimization in ('O0', 'O2') for ownership in ('none', 'arc', 'mrc')]


def qualify(repo: Path, compiler_root: Path, resource_root: Path, output: Path,
            *, objc_runtimes: dict[str, Path] | None = None) -> dict:
    repo, compiler_root = repo.resolve(strict=True), compiler_root.resolve(strict=True)
    resource_root, output = resource_root.resolve(strict=True), output.resolve()
    objc_runtimes = objc_runtimes or {}
    if set(objc_runtimes) - {'address,undefined', 'thread'}:
        raise ValueError('unknown native runtime instrumentation mode')
    disjoint(output, compiler_root)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    fixture = repo / 'tests/clang_foundation/task_coroutines'
    inputs = [Path(__file__).resolve(), *sorted(fixture.glob('*'))]
    inputs += sorted((repo / 'native/objc3c/src/runtime').rglob('*.h'))
    inputs += sorted((repo / 'native/objc3c/src/runtime/public').glob('*.hpp'))
    cases, commands = [], []
    policies = {}
    profiles = None

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
        profiles = RuntimeProfiles(repo, compiler_root, resource_root, output / 'runtime-profiles')
        for sanitizer, optimization, ownership in configurations():
            runtime_profile = profiles.get(sanitizer, optimization, objc_runtime=objc_runtimes.get(sanitizer))
            objc_runtime = runtime_profile.objc_runtime
            policy_key = sanitizer, optimization
            if policy_key not in policies:
                policies[policy_key] = SanitizerQualification(compiler_root / 'bin/clang++',
                    resource_root, output / 'detector-controls' / sanitizer.replace(',', '-') / optimization,
                    sanitizer, libraries=(runtime_profile.archive, objc_runtime),
                    additional_inputs=(Path(__file__),), base_environment=os.environ)
            policy = policies[policy_key]
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
                objects = [runtime_profile.archive]
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
                  'instrumentation': 'All canonical service objects and matching GNUstep dependency; coroutine and group-await clients',
                  'executions': cases, 'commands': commands,
                  'sanitizer_policy': {mode+'-'+opt: policy.finalize() for (mode, opt), policy in policies.items()},
                  'runtime_profiles': profiles.receipts(),
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
    parser.add_argument('--asan-objc-runtime', type=Path)
    parser.add_argument('--tsan-objc-runtime', type=Path)
    args = parser.parse_args(argv)
    report = qualify(args.repo, args.compiler_root, args.resource_root, args.output_dir,
                     objc_runtimes={mode: path for mode, path in
                         [('address,undefined', args.asan_objc_runtime), ('thread', args.tsan_objc_runtime)] if path is not None})
    print(json.dumps({'status': report['status'], 'executions': len(report['executions'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
