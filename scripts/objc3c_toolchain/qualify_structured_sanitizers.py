"""Instrument the structured runtime and its native threaded contract together."""
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


def qualify(repo: Path, compiler_root: Path, resource_root: Path, output: Path) -> dict:
    repo, compiler_root = repo.resolve(strict=True), compiler_root.resolve(strict=True)
    resource_root, output = resource_root.resolve(strict=True), output.resolve()
    disjoint(output, compiler_root)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    fixture = repo / 'tests/clang_foundation/structured_runtime/worker.cpp'
    inputs = [Path(__file__).resolve(), fixture, fixture.with_name('contract.c'),
                        fixture.with_name('actor_scheduling.cpp')]
    inputs += sorted((repo / 'native/objc3c/src/runtime').rglob('*.h'))
    cases, commands = [], []
    policies = {}
    profiles = None

    def run(label, argv, environment):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), env=environment, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=180)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': digest(log)})
        if result.returncode:
            raise ValueError('instrumented structured runtime failed: ' + label)
        require_clean_diagnostic(log.read_text(errors="replace"))

    try:
        profiles = RuntimeProfiles(repo, compiler_root, resource_root, output / 'runtime-profiles')
        for sanitizer in ('address,undefined', 'thread'):
            for optimization in ('O0', 'O2'):
                runtime_profile = profiles.get(sanitizer, optimization, objc_runtime=None)
                objc_runtime = runtime_profile.objc_runtime
                policy_key = sanitizer, optimization
                if policy_key not in policies:
                    policies[policy_key] = SanitizerQualification(compiler_root / 'bin/clang++',
                        resource_root, output / 'detector-controls' / sanitizer.replace(',', '-') / optimization,
                        sanitizer, libraries=(runtime_profile.archive, objc_runtime),
                        additional_inputs=(Path(__file__),), base_environment=os.environ)
                policy = policies[policy_key]
                policy.ensure_controls(optimization)
                with tempfile.TemporaryDirectory(prefix='objc3-structured-sanitizers-') as temporary:
                    work = Path(temporary)
                    label = sanitizer.replace(',', '-') + '-' + optimization
                    executable = work / label
                    env = policy.environment
                    run(label + '-build', [compiler_root / 'bin/clang++', '--no-default-config',
                        '-std=c++20', '-g', '-' + optimization, '-UNDEBUG', '-fno-omit-frame-pointer', '-fno-sanitize-recover=undefined',
                        '-fsanitize=' + sanitizer, '-resource-dir', resource_root, '-pthread', '-fuse-ld=lld',
                        '-DOBJC3_ACTOR_SCHEDULING_REGRESSIONS',
                        repo / 'tests/clang_foundation/structured_runtime/actor_scheduling.cpp',
                        '-I' + str(repo / 'native/objc3c/src'),
                        '-I' + str(repo / 'native/objc3c/src/runtime/public'),
                        fixture, runtime_profile.archive, objc_runtime,
                        '-Wl,-rpath,'+str(objc_runtime.parent), '-o', executable], env)
                    run(label + '-execute', [executable], env)
                    cases.append({'sanitizer': sanitizer, 'optimization': optimization,
                                  'executable_sha256': digest(executable)})
        report = {'schema': 'objc3c.structured-runtime-sanitizers.v1', 'status': 'PASS',
                  'compiler_root': str(compiler_root), 'compiler_sha256': digest(compiler_root / 'bin/clang'),
                  'sanitizer_resource_root': str(resource_root),
                  'instrumentation': 'All canonical service objects and matching GNUstep dependency; threaded structured-runtime clients',
                  'executions': cases, 'commands': commands,
                  'sanitizer_policy': {mode+'-'+opt: policy.finalize() for (mode, opt), policy in policies.items()},
                  'runtime_profiles': profiles.receipts(),
                  'source_hashes': {str(path.relative_to(repo)): digest(path) for path in inputs},
                  'sanitizer_runtime_sha256': {str(path.relative_to(resource_root)): digest(path)
                      for path in sorted(resource_root.rglob('libclang_rt.*san*.a'))}}
    except Exception as error:
        write_json(receipt, {'schema': 'objc3c.structured-runtime-sanitizers.v1', 'status': 'FAIL',
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
    args = parser.parse_args(argv)
    report = qualify(args.repo, args.compiler_root, args.resource_root, args.output_dir)
    print(json.dumps({'status': report['status'], 'executions': len(report['executions'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
