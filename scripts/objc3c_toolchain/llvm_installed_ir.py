"""Verify generated Objective-C 3 IR and native outputs with a sealed Linux SDK."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

from .binary_packages import digest, installed_shared_library
from .compiler_instrumentation import loaded_graph
from .linux_host import native_loader, native_target
from .llvm_ir import instruction_body, module_facts
from .llvm_ir_execution import Execution
from .package import verify_sdk
from .sdk_example import environment, runtime_link_flags
from .semantic_model import generate, interpret, mismatch
from .semantic_oracles import source_bundle

SEEDS = (7, 19, 41)


def qualify(sdk: Path, output: Path, *, seal_sha256: str) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    if (not re.fullmatch('[0-9a-f]{64}', seal_sha256) or output.exists() or
            output.is_relative_to(sdk) or sdk.is_relative_to(output)):
        raise ValueError('installed IR assurance requires an exact SDK seal and fresh separate evidence')
    admitted = verify_sdk(sdk, seal_sha256)
    target = native_target()
    if admitted.get('target') != target:
        raise ValueError('installed IR assurance requires the native Linux SDK target')
    tools = {name: sdk / 'bin' / name for name in ('clang++', 'ld.lld', 'llc', 'llvm-dwarfdump')}
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.installed-llvm-ir.v1', 'status': 'RUNNING', 'target': target,
              'sdk': {key: admitted[key] for key in ('seal_sha256', 'source_build_id',
                                                    'patch_digest', 'build_config_digest')},
              'tools': {name: {'path': str(path), 'sha256': digest(path)} for name, path in tools.items()},
              'seeds': list(SEEDS), 'cases': []}
    env = environment(sdk)
    executor = Execution(output, env, report)
    try:
        for seed in SEEDS:
            program = generate(seed)
            for optimization in ('O0', 'O2'):
                verify_case(executor, sdk, tools, target, seed, program, optimization)
        verify_sdk(sdk, seal_sha256)
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        executor.save()
    return report


def verify_case(executor, sdk, tools, target, seed, program, optimization):
    name = f'seed-{seed}-{optimization}'
    output = executor.output / name
    source_bundle(program, output, optimization=optimization)
    clang = tools['clang++']
    compile_flags = [f'--target={target}', '-x', 'objective-c++', '-std=c++20',
                     '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fobjc-arc',
                     '-fobjc-arc-exceptions', '-fexceptions', '-fobjc-exceptions', '-fblocks',
                     '-isystem', sdk / 'include', '-g', '-'+optimization, '-UNDEBUG']
    modules, driver_objects, backend_objects = {}, [], []
    for unit in ('provider', 'consumer'):
        source = output / (unit+'.mm')
        frontend, optimized, driver, backend = (output / (unit+suffix) for suffix in
                                                ('.frontend.ll', '.optimized.ll', '.driver.o', '.backend.o'))
        stage = name+'-'+unit
        executor.run(stage+'-frontend', [clang, *compile_flags, '-Xclang', '-disable-llvm-passes',
            '-S', '-emit-llvm', source, '-o', frontend], outputs=[frontend])
        executor.run(stage+'-optimized', [clang, *compile_flags, '-S', '-emit-llvm', source, '-o', optimized],
                     outputs=[optimized])
        before, after = frontend.read_text(), optimized.read_text()
        facts = module_facts(before, target, require_optimizable=optimization == 'O2')
        module_facts(after, target)
        modules[unit] = {'frontend': facts, 'instruction_bodies_changed': instruction_body(before) != instruction_body(after)}
        executor.run(stage+'-driver', [clang, *compile_flags, '-c', source, '-o', driver], outputs=[driver])
        executor.run(stage+'-backend', [tools['llc'], '-verify-machineinstrs', '-relocation-model=pic',
            '-filetype=obj', optimized, '-o', backend], outputs=[backend])
        for route, obj in (('driver', driver), ('backend', backend)):
            executor.run(stage+'-'+route+'-dwarf', [tools['llvm-dwarfdump'], '--verify', obj],
                         stage='debug', contains=('No errors.',))
        driver_objects.append(driver)
        backend_objects.append(backend)
    if optimization == 'O2' and not any(row['instruction_bodies_changed'] for row in modules.values()):
        raise ValueError('installed optimizer did not transform a generated IR case')
    expected = interpret(program)
    routes = {}
    runtime = installed_shared_library(sdk / 'lib', 'libobjc.so')
    for route, objects in (('driver', driver_objects), ('backend', backend_objects)):
        binary = output / (route+'.bin')
        executor.run(name+'-'+route+'-link', [clang, '--ld-path='+str(tools['ld.lld']),
            *objects, '-L'+str(sdk / 'lib'), '-Wl,-rpath,'+str(sdk / 'lib'),
            *runtime_link_flags(sdk), '-o', binary], outputs=[binary])
        graph = loaded_graph(binary, native_loader(), [runtime], required=(runtime,),
            run=lambda suffix, argv: executor.run(name+'-'+route+'-'+suffix, argv))
        observed = json.loads(executor.run(name+'-'+route+'-execute', [binary], stage='runtime-oracle'))
        disagreement = mismatch(expected, observed)
        if disagreement:
            raise ValueError(f'{name} {route} disagreed with the independent oracle: {disagreement}')
        routes[route] = {'sha256': digest(binary), 'loaded_graph': graph, 'observation': observed, 'status': 'PASS'}
    executor.report['cases'].append({'name': name, 'source_sha256': digest(output / 'program.json'),
                                     'modules': modules, 'routes': routes, 'status': 'PASS'})
    executor.save()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk', type=Path, required=True)
    parser.add_argument('--seal-sha256', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    report = qualify(args.sdk, args.output, seal_sha256=args.seal_sha256)
    print(json.dumps({'status': report['status'], 'cases': len(report['cases'])}))


if __name__ == '__main__':
    main()
