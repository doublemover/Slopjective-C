"""Run the reviewed ownership oracle through a relocated, locked re-export.

Run with PYTHONPATH=. from the checkout. The facade can be the SDK's objc3c or
a separately rebuilt binary compatible with its driver and runtime.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from scripts.objc3c_toolchain.binary_packages import digest
from scripts.objc3c_toolchain.linux_host import native_target, require_elf
from scripts.objc3c_toolchain.llvm_ir_execution import Execution
from scripts.objc3c_toolchain.package import verify_sdk
from scripts.objc3c_toolchain.sdk_example import environment
from scripts.objc3c_toolchain.semantic_interface_locks import exercise_reexport
from scripts.objc3c_toolchain.semantic_model import generate
from scripts.objc3c_toolchain.semantic_oracles import EXPORTS, source_bundle



SEED = 7


def qualify(sdk: Path, facade: Path, output: Path):
    sdk, facade, output = sdk.resolve(strict=True), facade.resolve(strict=True), output.resolve()
    if output.is_relative_to(sdk) or sdk.is_relative_to(output) or output.exists():
        raise ValueError('qualification requires a fresh output directory disjoint from the SDK')
    require_elf(facade)
    target = native_target()
    manifest = verify_sdk(sdk)
    if manifest['target'] != target:
        raise ValueError('SDK target differs from the native host')
    clang, services, runtime = (sdk / name for name in
        ('bin/clang++', 'lib/libobjc3_runtime_services.a', 'lib/libobjc.so'))
    report = {'schema': 'objc3c.semantic-source-free-dependency.v1', 'status': 'running',
              'target': target, 'seed': SEED, 'sdk_seal': manifest['seal_sha256'],
              'inputs': {str(path): digest(path) for path in (facade, clang, services, runtime)},
              'ownership_change': {'declaration': 'oracle_make', 'before': 'ns_returns_retained',
                                   'after': 'ns_returns_not_retained'}, 'cases': []}
    execution = Execution(output, environment(sdk), report)
    execution.save()
    program = generate(SEED)
    try:
        for optimization in ('O0', 'O2'):
            report['cases'].extend(exercise_reexport(output / optimization, execution.run,
                facade=facade, clang=clang, services=services, runtime=runtime, flags=[
                    '--no-default-config', '--target=' + target, '-fexceptions', '-fobjc-exceptions',
                    '-fobjc-arc-exceptions', '-fblocks', '-isystem', sdk / 'include'],
                program=program, origin={'seed': SEED}, prepare_bundle=source_bundle, exports=EXPORTS,
                optimization=optimization, frontend_options=('--sdk-root', sdk)))
        report['status'] = 'PASS'
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        execution.save()
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--facade', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.facade, args.output)
    print(json.dumps({'status': result['status'], 'cases': len(result['cases'])}))
