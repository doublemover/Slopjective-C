"""Instrument both languages and task services for the typed async C frame ABI."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from .binary_packages import digest, disjoint, write_json
from .qualify_async_c_abi import qualify as qualify_bridge
from .runtime_instrumentation import RuntimeProfiles


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
    compiler = (compiler or sdk / 'bin/clang').resolve(strict=True)
    headers = headers or sdk / 'include/objc3c/runtime'
    fixture = (fixture or sdk / 'share/objc3c/examples/async-c-abi').resolve(strict=True)
    headers = headers.resolve(strict=True)
    runtime = repo / 'native/objc3c/src/runtime'
    # The instrumented service implementation and selected public ABI must agree.
    if not (digest(headers / 'objc3_runtime_services.h') == digest(runtime / 'public/objc3_runtime_services.h')):
        raise ValueError("qualify_async_c_abi_sanitizers: required semantic check failed: digest(headers / 'objc3_runtime_services.h') == digest(runtime / 'public/objc3_runtime_services.h')")
    if not (digest(headers / 'objc3_task.hpp') == digest(runtime / 'public/objc3_task.hpp')):
        raise ValueError("qualify_async_c_abi_sanitizers: required semantic check failed: digest(headers / 'objc3_task.hpp') == digest(runtime / 'public/objc3_task.hpp')")
    commands, results = [], []
    profiles = None
    try:
        profiles = RuntimeProfiles(repo, compiler.resolve(strict=True).parent.parent,
                                   resource_root, output / 'runtime-profiles')
        for sanitizer in ('address,undefined', 'thread'):
            profile = profiles.get(sanitizer, 'O1')
            label = sanitizer.replace(',', '-')
            result = qualify_bridge(sdk, output / label, compiler=compiler,
                fixture=fixture, headers=headers, runtime_library=profile.archive,
                objc_runtime=profile.objc_runtime, sanitizer=sanitizer, resource_root=resource_root)
            results.append({'sanitizer': sanitizer, 'executions': result['executions'],
                            'service_library_sha256': digest(profile.archive),
                            'qualification_sha256': digest(output / label / 'qualification.json')})
        report = {'schema': 'objc3c.async-c-abi-sanitizer-qualification.v1',
                  'status': 'PASS', 'sdk_seal': manifest['seal_sha256'],
                  'input_binding': 'development-overlay' if development else 'sealed-sdk',
                  'service_optimization': 'O1',
                  'executions': sum(r['executions'] for r in results),
                  'sanitizers': results, 'commands': commands,
                  'runtime_profiles': profiles.receipts(),
                  'compiler_sha256': digest(compiler.resolve()),
                  'sanitizer_resource_root': str(resource_root),
                  'sanitizer_runtime_sha256': {str(p.relative_to(resource_root)): digest(p)
                      for p in sorted(resource_root.rglob('libclang_rt.*san*.a'))},
                  'source_hashes': {str(p.relative_to(repo)): digest(p)
                      for p in [Path(__file__).resolve(), *sorted(runtime.rglob('*.h'))]},
                  'header_hashes': {name: digest(headers / name)
                      for name in ('objc3_task.hpp', 'objc3_runtime_services.h')}}
    except Exception as error:
        write_json(receipt, {'schema': 'objc3c.async-c-abi-sanitizer-qualification.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'],
            'sanitizers': results, 'commands': commands,
                  'runtime_profiles': profiles.receipts(), 'error': str(error)})
        raise
    write_json(receipt, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--resource-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    for name in ('compiler', 'fixture', 'headers'):
        parser.add_argument('--'+name, type=Path)
    args = parser.parse_args()
    report = qualify(args.repo, args.sdk_root, args.resource_root, args.output_dir,
                     compiler=args.compiler, fixture=args.fixture, headers=args.headers)
    print(json.dumps({'status': report['status'], 'executions': report['executions']}))


if __name__ == '__main__':
    main()
