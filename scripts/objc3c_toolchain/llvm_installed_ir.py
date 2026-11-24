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
from .sdk_example import environment, native_module_compile_flags, runtime_link_flags
from .semantic_model import generate, interpret, mismatch
from .semantic_oracles import EXPORTS, source_bundle

SEEDS = (7, 19, 41)
INTERFACE_MODULE = 'oracle.Provider'
INTERFACE_PACKAGE = 'semantic-oracle-v1'


def qualify(sdk: Path, output: Path, *, seal_sha256: str) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    if (not re.fullmatch('[0-9a-f]{64}', seal_sha256) or output.exists() or
            output.is_relative_to(sdk) or sdk.is_relative_to(output)):
        raise ValueError('installed IR assurance requires an exact SDK seal and fresh separate evidence')
    admitted = verify_sdk(sdk, seal_sha256)
    target = native_target()
    if admitted.get('target') != target:
        raise ValueError('installed IR assurance requires the native Linux SDK target')
    tools = {name: sdk / 'bin' / name for name in ('clang++', 'objc3c', 'ld.lld', 'llc', 'llvm-dwarfdump')}
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
        verify_case(executor, sdk, tools, target, SEEDS[0], generate(SEEDS[0]), 'O2', transport='interface')
        verify_sdk(sdk, seal_sha256)
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        executor.save()
    return report


def verify_case(executor, sdk, tools, target, seed, program, optimization, *, transport='source'):
    name = f'seed-{seed}-{optimization}'
    source_free = transport == 'interface'
    if source_free:
        name += '-interface'
    output = executor.output / name
    source_bundle(program, output, transport=transport, optimization=optimization, origin={'seed': seed})
    clang = tools['clang++']
    compile_flags = [f'--target={target}', '-x', 'objective-c++', '-std=c++20',
                     '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fobjc-arc',
                     '-fobjc-arc-exceptions', '-fexceptions', '-fobjc-exceptions', '-fblocks',
                     '-isystem', sdk / 'include', '-g', '-'+optimization, '-UNDEBUG']
    interface_evidence = {}
    if source_free:
        interface = output / (INTERFACE_MODULE+'.objc3i')
        sidecar = interface.with_suffix('.objc3i.json')
        executor.run(name+'-publish', [tools['objc3c'], output / 'provider.mm', '--objc3', '--objcxx', '--arc',
            '--emit-interface', interface, '--module', INTERFACE_MODULE, '--package', INTERFACE_PACKAGE,
            '--module-source-root', output, *(part for export in EXPORTS for part in ('--export-declaration', export)),
            '--', *compile_flags], outputs=[interface, sidecar], stage='frontend-interface')
        module = json.loads(sidecar.read_text())['module']
        lock = {'name': module['canonical_name'], 'fingerprint': module['semantic_fingerprint'],
                'package': module['package_identity']}
        interface_evidence = {'lock': lock,
                              'artifacts': {str(path.relative_to(executor.output)): digest(path)
                                            for path in (interface, sidecar)},
                              'records': {}, 'retained_provider': {}}
    modules, driver_objects, backend_objects = {}, [], []
    for unit in ('provider', 'consumer'):
        source = output / (unit+'.mm')
        frontend, optimized, driver, backend = (output / (unit+suffix) for suffix in
                                                ('.frontend.ll', '.optimized.ll', '.driver.o', '.backend.o'))
        stage = name+'-'+unit
        imported = source_free and unit == 'consumer'
        if imported:
            # Keep replay inputs outside the consumer's source and module search directory.
            retained = executor.output / 'retained-provider' / name
            retained.mkdir(parents=True)
            for filename in ('provider.mm', 'oracle.h'):
                saved = (output / filename).rename(retained / filename)
                interface_evidence['retained_provider'][str(saved.relative_to(executor.output))] = digest(saved)

        def emit(phase, destination, *, extra=()):
            object_output = phase == 'driver'
            if imported:
                record = output / (phase+'.dependencies.json')
                executor.run(stage+'-'+phase, [tools['objc3c'], source, '--objc3', '--objcxx', '--arc',
                    '--source-imports', '--module-search-path', output, '--import-module',
                    lock['name'], lock['fingerprint'], lock['package'], '--emit-dependency-record', record,
                    '--emit-object' if object_output else '--emit-llvm', destination, '--', *compile_flags,
                    *native_module_compile_flags('objective-c++', output / phase), *extra],
                    outputs=[destination, record], stage='frontend-interface')
                selected = json.loads(record.read_text())['modules']
                if len(selected) != 1 or any(selected[0][key] != value for key, value in lock.items()):
                    raise ValueError('installed IR consumer did not resolve the exact source-free dependency lock')
                interface_evidence['records'][str(record.relative_to(executor.output))] = digest(record)
            else:
                action = ['-c'] if object_output else ['-S', '-emit-llvm']
                executor.run(stage+'-'+phase, [clang, *compile_flags, *extra, *action, source, '-o', destination],
                             outputs=[destination], stage='frontend-lowering')

        emit('frontend', frontend, extra=('-Xclang', '-disable-llvm-passes'))
        emit('optimized', optimized)
        before, after = frontend.read_text(), optimized.read_text()
        facts = module_facts(before, target, require_optimizable=optimization == 'O2')
        module_facts(after, target)
        modules[unit] = {'frontend': facts, 'instruction_bodies_changed': instruction_body(before) != instruction_body(after)}
        emit('driver', driver)
        executor.run(stage+'-backend', [tools['llc'], '-verify-machineinstrs', '-relocation-model=pic',
            '-filetype=obj', optimized, '-o', backend], outputs=[backend])
        for route, obj in (('driver', driver), ('backend', backend)):
            executor.run(stage+'-'+route+'-dwarf', [tools['llvm-dwarfdump'], '--verify', obj],
                         stage='debug', contains=('No errors.',))
        driver_objects.append(driver)
        backend_objects.append(backend)
    optimization_witnesses = [modules['consumer']] if source_free else modules.values()
    if optimization == 'O2' and not any(row['instruction_bodies_changed'] for row in optimization_witnesses):
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
    row = {'name': name, 'transport': transport, 'source_sha256': digest(output / 'program.json'),
           'modules': modules, 'routes': routes, 'status': 'PASS'}
    if source_free:
        row['interface'] = interface_evidence
    executor.report['cases'].append(row)
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
