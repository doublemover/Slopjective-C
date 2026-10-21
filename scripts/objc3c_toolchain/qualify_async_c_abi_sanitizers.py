"""Instrument both languages and task services for the typed async C frame ABI."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import subprocess

from .binary_packages import digest, disjoint, write_json
from .qualify_async_c_abi import qualify as qualify_bridge


def qualify(repo: Path, sdk: Path, resource_root: Path, output: Path, *, compiler: Path | None = None,
            fixture: Path | None = None, headers: Path | None = None) -> dict:
    repo, sdk, output = repo.resolve(strict=True), sdk.resolve(strict=True), output.resolve()
    resource_root = resource_root.resolve(strict=True)
    disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    development = any(value is not None for value in (compiler, fixture, headers))
    compiler = compiler or sdk / 'bin/clang'
    headers = headers or sdk / 'include/objc3c/runtime'
    fixture = fixture or sdk / 'share/objc3c/examples/async-c-abi'
    runtime = repo / 'native/objc3c/src/runtime'
    # The instrumented service implementation and selected public ABI must agree.
    assert digest(headers / 'objc3_runtime_services.h') == digest(runtime / 'public/objc3_runtime_services.h')
    assert digest(headers / 'objc3_task.hpp') == digest(runtime / 'public/objc3_task.hpp')
    sources = [runtime / name for name in (
        'services/error_service.cpp', 'services/objc_task_error.cpp',
        'services/carrier_service.cpp', 'concurrency/service_executor.cpp',
        'concurrency/service_task.cpp', 'concurrency/service_group.cpp',
        'concurrency/service_actor.cpp', 'concurrency/service_public.cpp')]
    commands, results = [], []

    def run(label, argv, cwd):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=os.environ.copy(),
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=180)
        commands.append({'name': label, 'exit_code': result.returncode,
                         'log_sha256': digest(log)})
        if result.returncode:
            raise ValueError('instrumented async C ABI runtime failed: ' + label)

    try:
        for sanitizer in ('address,undefined', 'thread'):
            label = sanitizer.replace(',', '-')
            objects = output / (label + '-objects')
            objects.mkdir(exist_ok=True)
            for old in objects.glob('*.o'):
                old.unlink()
            run(label + '-runtime-build', [compiler, '--no-default-config',
                '-std=c++20', '-O1', '-g', '-fno-omit-frame-pointer', '-pthread',
                '-resource-dir', resource_root,
                '-fsanitize=' + sanitizer, '-I' + str(repo / 'native/objc3c/src'),
                '-c', *sources], objects)
            library = output / (label + '-services.a')
            library.unlink(missing_ok=True)
            run(label + '-runtime-archive', [compiler.parent / 'llvm-ar', 'rcs',
                library, *sorted(objects.glob('*.o'))], objects)
            result = qualify_bridge(sdk, output / label, compiler=compiler,
                fixture=fixture, headers=headers, runtime_library=library,
                sanitizer=sanitizer, resource_root=resource_root)
            results.append({'sanitizer': sanitizer, 'executions': result['executions'],
                            'service_library_sha256': digest(library),
                            'qualification_sha256': digest(output / label / 'qualification.json')})
        report = {'schema': 'objc3c.async-c-abi-sanitizer-qualification.v1',
                  'status': 'PASS', 'sdk_seal': manifest['seal_sha256'],
                  'input_binding': 'development-overlay' if development else 'sealed-sdk',
                  'service_optimization': 'O1',
                  'executions': sum(r['executions'] for r in results),
                  'sanitizers': results, 'commands': commands,
                  'compiler_sha256': digest(compiler.resolve()),
                  'sanitizer_resource_root': str(resource_root),
                  'sanitizer_runtime_sha256': {str(p.relative_to(resource_root)): digest(p)
                      for p in sorted(resource_root.rglob('libclang_rt.*san*.a'))},
                  'source_hashes': {str(p.relative_to(repo)): digest(p)
                      for p in sources + sorted(runtime.rglob('*.h'))},
                  'header_hashes': {name: digest(headers / name)
                      for name in ('objc3_task.hpp', 'objc3_runtime_services.h')}}
    except Exception as error:
        write_json(receipt, {'schema': 'objc3c.async-c-abi-sanitizer-qualification.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'],
            'sanitizers': results, 'commands': commands, 'error': str(error)})
        raise
    write_json(receipt, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--resource-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    report = qualify(args.repo, args.sdk_root, args.resource_root, args.output_dir)
    print(json.dumps({'status': report['status'], 'executions': report['executions']}))


if __name__ == '__main__':
    main()
