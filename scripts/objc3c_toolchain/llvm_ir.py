"""Verify real Objective-C 3 frontend IR and execute its existing semantic oracles.

This development lane preserves production-driver O0/O2 and ThinLTO execution.
The additional split opt/llc route is recorded separately, never called an SDK.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import shutil
import sys
import time

from .binary_packages import digest, write_json
from .compiler_instrumentation import loaded_graph
from .llvm_ir_execution import Execution, StageFailure
from .llvm_ir_inventory import stages, thinlto_modules, verify as verify_inventory
from .llvm_ir_controls import qualify as qualify_controls
from .llvm_tools import identity, qualify as qualify_tools, selected, verify_receipt


def corpus(repo: Path) -> dict:
    document = json.loads((repo / 'toolchains/llvm/ir-corpus.json').read_text())
    rows = document['cases']
    if (document.get('schema') != 'objc3c.llvm-ir-corpus.v1' or
            document.get('target') != 'x86_64-unknown-linux-gnu' or not rows or
            len({row['name'] for row in rows}) != len(rows) or
            set(document['families']) != {'carriers', 'calls', 'cleanup', 'async', 'keypaths'} or
            {name for row in rows for name in row['families']} != set(document['families'])):
        raise ValueError('incomplete or mismatched native IR corpus')
    for row in rows:
        if (not re.fullmatch(r'[a-z][a-z0-9-]+', row['name']) or
                row['transport'] not in ('source', 'pch', 'interface') or
                row['ownership'] not in ('arc', 'mrc') or
                row['language'] not in ('objective-c', 'objective-c++') or
                row['standard'] not in ('c17', 'c++20', 'c++23') or
                not {'O0', 'O2'}.issubset(row['optimizations']) or
                set(row['optimizations']) - {'O0', 'O2', 'Os'} or
                len(set(row['optimizations'])) != len(row['optimizations']) or not row['oracle']):
            raise ValueError('invalid selected native IR case')
        folder = (repo / 'tests/clang_foundation' / row['directory']).resolve(strict=True)
        if not folder.is_relative_to(repo / 'tests/clang_foundation'):
            raise ValueError('IR corpus directory escaped the authored fixture tree')
        for name in [*row['sources'], *([row['header']] if row['transport'] == 'pch' else []),
                     *([row['module_source']] if row['transport'] == 'interface' else [])]:
            if Path(name).name != name or not (folder / name).is_file():
                raise ValueError('missing or escaped authored IR witness')
    if {row['transport'] for row in rows} != {'source', 'pch', 'interface'}:
        raise ValueError('IR corpus lost source, PCH or semantic-interface coverage')
    return document


def module_facts(text: str, target: str, *, require_optimizable=False) -> dict:
    triple = re.search(r'^target triple = "([^"]+)"', text, re.M)
    layout = re.search(r'^target datalayout = "([^"]+)"', text, re.M)
    definitions = re.findall(r'^define\s+.*$', text, re.M)
    groups = dict(re.findall(r'^attributes #(\d+) = \{([^\n]*)\}', text, re.M))
    optimizable = []
    for line in definitions:
        group = re.search(r'#(\d+)', line)
        if 'optnone' not in line and (not group or 'optnone' not in groups.get(group[1], '')):
            optimizable.append(line)
    if not triple or triple[1] != target or not layout or not definitions:
        raise ValueError('frontend module lacks its declared target, data layout or function definitions')
    if require_optimizable and not optimizable:
        raise ValueError('all-optnone frontend module cannot exercise the selected optimizer')
    return {'triple': triple[1], 'data_layout': layout[1], 'definitions': len(definitions),
            'optimizable_definitions': len(optimizable),
            'target_attributes': sorted(set(re.findall(r'"target-(?:cpu|features)"="[^"]*"', text)))}


def instruction_body(text: str) -> list[str]:
    """Compare canonical instruction bodies; debug renumbering is not a transform."""
    result, inside = [], False
    for line in text.splitlines():
        if line.startswith('define '):
            inside = True
        elif line == '}':
            inside = False
        elif inside:
            value = re.sub(r',? ![\w.]+ !\d+', '', line).split(';', 1)[0].strip()
            if value:
                result.append(value)
    return result


def pipeline_options(optimization: str, ownership: str) -> list[str]:
    pipeline = 'O2' if optimization == 'Os' else optimization
    if pipeline not in ('O0', 'O2') or ownership not in ('arc', 'mrc'):
        raise ValueError('unsupported native IR optimization/ownership policy')
    result = ['-passes=default<'+pipeline+'>', '-verify-each']
    if ownership == 'arc' and optimization != 'O0':
        # Match Clang BackendUtil.cpp's ARC extension points exactly. Generic
        # opt alone lacks these language callbacks and can miscompile raw ARC IR.
        result += ['-passes-ep-pipeline-start=function(objc-arc-expand)',
                   '-passes-ep-scalar-optimizer-late=objc-arc']
    return result


def capability_controls(repo: Path, output: Path, tools: dict, run) -> dict:
    fixtures = repo / 'tests/tooling/fixtures/llvm_assurance'
    work = output / 'controls'
    work.mkdir()
    for source in fixtures.iterdir():
        shutil.copyfile(source, work / source.name)
    valid = work / 'valid.ll'
    run('control-valid-ir', [tools['opt'], '-passes=verify', '-disable-output', valid])
    run('control-invalid-ir', [tools['opt'], '-passes=verify', '-disable-output', work / 'dominance-invalid.ll'],
        expected=1, contains=('Instruction does not dominate all uses!', 'input module is broken'))
    for optimization in ('O0', 'O2', 'Os'):
        # LLVM 23's size policy lives in frontend-emitted optsize attributes;
        # default<Os> is explicitly rejected by this fork's PassBuilder.
        pipeline = 'O2' if optimization == 'Os' else optimization
        bitcode = work / (optimization+'.bc')
        obj = work / (optimization+'.o')
        run('control-pipeline-'+optimization, [tools['opt'], '-passes=default<'+pipeline+'>',
            '-verify-each', valid, '-o', bitcode], outputs=[bitcode])
        run('control-backend-'+optimization, [tools['llc'], '-verify-machineinstrs',
            '-O2' if optimization == 'Os' else '-'+optimization,
            '-filetype=obj', bitcode, '-o', obj], outputs=[obj])
    run('control-backend-invalid', [tools['llc'], '-mtriple=x86_64-unknown-linux-gnu',
        '-run-pass=none', '-verify-machineinstrs', '-filetype=null', work / 'backend-invalid.mir'],
        expected=-6, contains=('Bad machine code: PHI operand is not live-out from predecessor', 'func0'))
    run('control-absent-pass', [tools['opt'], '-passes=objc3_nonexistent_control', '-disable-output', valid],
        expected=1, contains=('unknown pass name', 'objc3_nonexistent_control'))
    run('control-arc-extension-points', [tools['opt'], *pipeline_options('O2', 'arc'),
        '-debug-pass-manager', '-disable-output', valid],
        contains=('ObjCARCExpandPass', 'ObjCARCOptPass'))
    return {'status': 'PASS', 'valid_ir': digest(valid),
            'invalid_ir': digest(work / 'dominance-invalid.ll'),
            'backend_control': digest(work / 'backend-invalid.mir'),
            'backend_signature': 'PHI operand is not live-out from predecessor',
            'pipelines': ['default<O0>', 'default<O2>'],
            'size_canary': 'Production -Os; default<O2> honors frontend optsize attributes'}


def qualify(repo: Path, output: Path, *, runtime: Path, objc_runtime: Path,
            facade: Path, timeout=180, tools_receipt: Path | None = None) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('native IR assurance requires a fresh repository out evidence directory')
    runtime, objc_runtime, facade = (path.resolve(strict=True) for path in (runtime, objc_runtime, facade))
    plan, tools, environment = selected(repo)
    stage = Path(plan['paths']['stage'])
    target = corpus(repo)['target']
    document = corpus(repo)
    report = {'schema': 'objc3c.llvm-ir-qualification.v1', 'status': 'running',
        'subject': 'real-native-frontend-ir', 'target': target,
        'started_seconds': time.time(), 'cases': [], 'expected_cases': [],
        'boundaries': ['Verification establishes well-formedness, not semantic equivalence.',
                       'Split opt/llc is additional to the production driver and actual ThinLTO.',
                       'DWARF verification does not establish debugger variable correctness.',
                       'This run does not qualify an expensive-checks foundation or SDK archive.']}
    for row in document['cases']:
        for optimization in row['optimizations']:
            report['expected_cases'].append(row['name']+'-'+optimization)
    report['expected_stages'] = [name for row in document['cases'] for optimization in row['optimizations']
                                 for name in stages(row, optimization)]
    output.mkdir(parents=True)
    executor = Execution(output, environment, report, timeout=timeout)
    run = executor.run
    input_files = [repo / 'toolchains/llvm/ir-corpus.json', repo / 'toolchains/llvm/assurance-tools.json',
                   Path(__file__), Path(__file__).with_name('llvm_ir_execution.py'),
                   *(Path(__file__).with_name(name+'.py') for name in
                     ('llvm_tools', 'llvm_ir_inventory', 'llvm_ir_controls', 'llvm_reduce')),
                   runtime, objc_runtime, facade]
    for tree in (repo / 'native/objc3c/src/runtime/public', repo / 'stdlib/include'):
        input_files.extend(path for path in tree.rglob('*') if path.is_file())
    report['inputs'] = {str(path): digest(path) for path in input_files}
    report['runtime'] = {'service_archive': str(runtime), 'objc_runtime': str(objc_runtime)}
    report['permitted_environment'] = environment
    try:
        tool_receipt, tools, _ = qualify_tools(repo, run=run, receipt=tools_receipt)
        write_json(output / 'tools.json', tool_receipt)
        report['tool_identity'] = tool_receipt['identity']
        report['controls'] = capability_controls(repo, output, tools, run)
        # Imported source runs inside the already qualified embedding facade.
        loaded_graph(facade, Path('/lib64/ld-linux-x86-64.so.2').resolve(),
            [(stage / 'lib/libLLVM.so').resolve(), (stage / 'lib/libclang-cpp.so').resolve()],
            required=((stage / 'lib/libLLVM.so').resolve(),),
            run=lambda name, cmd: run('facade-'+name, cmd))
        headers = repo / 'native/objc3c/src/runtime/public'
        for case in document['cases']:
            folder = repo / 'tests/clang_foundation' / case['directory']
            for optimization in case['optimizations']:
                name = case['name']+'-'+optimization
                work = output / name
                work.mkdir()
                sources = work / 'sources'
                sources.mkdir()
                for path in folder.iterdir():
                    if path.is_file() and path.suffix in ('.m', '.mm', '.cpp', '.h', '.hpp'):
                        shutil.copyfile(path, sources / path.name)
                        report['inputs'][str(path.resolve())] = digest(path)
                (work / 'include/objc3c').mkdir(parents=True)
                (work / 'include/objc3c/runtime').symlink_to(headers, target_is_directory=True)
                for tree in (stage / 'include', repo / 'stdlib/include', objc_runtime.parent.parent / 'include'):
                    for entry in tree.iterdir():
                        if entry.name == 'objc3c':
                            continue
                        view = work / 'include' / entry.name
                        if view.is_symlink():
                            view.unlink()
                        view.symlink_to(entry, target_is_directory=entry.is_dir())
                flags = ['--no-default-config', '--target='+target, '-x', case['language'],
                    '-std='+case['standard'], '-'+optimization, '-g', '-UNDEBUG', '-fblocks',
                    '-fexceptions', '-fobjc-exceptions', '-fobjc-arc-exceptions',
                    '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2',
                    '-fobjc-arc' if case['ownership'] == 'arc' else '-fno-objc-arc',
                    '-fmodules', '-fmodules-cache-path='+str(work / 'modules'),
                    '-I'+str(work / 'include'), '-I'+str(headers), '-I'+str(sources)]
                if '++' in case['language']:
                    flags.append('-fcoro-aligned-allocation')
                extra = []
                if case['transport'] == 'pch':
                    pch = work / 'fixture.pch'
                    marker = '-D'+case['marker']
                    run(name+'-pch', [tools['clang'], *flags, '-x', case['language']+'-header', marker,
                        sources / case['header'], '-o', pch], outputs=[pch], stage='frontend-pch')
                    extra = [marker, '-include-pch', pch]
                interface_extra = []
                if case['transport'] == 'interface':
                    interface = work / (case['module']+'.objc3i')
                    run(name+'-interface', [facade, sources / case['module_source'], '--objc3', '--arc',
                        '--emit-interface', interface, '--module', case['module'], '--package', case['package'],
                        '--', *flags], outputs=[interface, interface.with_suffix('.objc3i.json')],
                        stage='frontend-interface')
                    module = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                    interface_extra = ['--source-imports', '--module-search-path', work, '--import-module',
                        case['module'], module['semantic_fingerprint'], case['package']]
                row = {'name': name, 'status': 'running', 'configuration': case,
                       'units': [], 'expected_units': case['sources'], 'executions': []}
                report['cases'].append(row)
                objects = {'production': [], 'split': [], 'thinlto': []}
                for index, source_name in enumerate(case['sources']):
                    source = sources / source_name
                    unit_name = name+'-'+str(index)
                    unit = work / ('unit-'+str(index))
                    unit.mkdir()
                    imported = case['transport'] == 'interface' and source_name.endswith('import.m')
                    def compile_to(label, destination, mode, additions=()):
                        args = [*flags, *extra, *additions]
                        if imported:
                            argv = [facade, source, '--objc3', '--arc', *interface_extra,
                                    '--emit-llvm' if mode == 'ir' else '--emit-object', destination, '--', *args]
                        else:
                            argv = [tools['clang'], *args, *(['-S', '-emit-llvm'] if mode == 'ir' else ['-c']),
                                    source, '-o', destination]
                        run(unit_name+'-'+label, argv, outputs=[destination], stage='frontend-lowering')
                    # Record the native driver's cc1 route in addition to the
                    # actual command, so reproductions keep target/codegen flags.
                    if not imported:
                        run(unit_name+'-cc1', [tools['clang'], *flags, *extra, '-###', '-c', source])
                    raw, baseline, base_text = unit / 'frontend.ll', unit / 'baseline.bc', unit / 'baseline.ll'
                    compile_to('frontend-ir', raw, 'ir', ['-Xclang', '-disable-llvm-passes'])
                    facts = module_facts(raw.read_text(), target, require_optimizable=optimization != 'O0')
                    if optimization == 'Os' and 'optsize' not in raw.read_text():
                        raise StageFailure(unit_name, 'infrastructure', 'size canary lost frontend optsize policy')
                    run(unit_name+'-input-verifier', [tools['opt'], '-passes=verify', raw, '-o', baseline],
                        outputs=[baseline], stage='input-verifier')
                    run(unit_name+'-baseline-dis', [tools['llvm-dis'], baseline, '-o', base_text], outputs=[base_text])
                    optimized, opt_text = unit / 'optimized.bc', unit / 'optimized.ll'
                    run(unit_name+'-optimizer', [tools['opt'], *pipeline_options(optimization, case['ownership']),
                        baseline, '-o', optimized], outputs=[optimized], stage='pass-verifier',
                        timeout=case.get('optimizer_timeout_seconds', timeout))
                    run(unit_name+'-optimized-dis', [tools['llvm-dis'], optimized, '-o', opt_text], outputs=[opt_text])
                    split = unit / 'split.o'
                    backend_level = 'O2' if optimization == 'Os' else optimization
                    run(unit_name+'-backend', [tools['llc'], '-verify-machineinstrs', '-'+backend_level,
                        '-mtriple='+target, '-relocation-model=pic', '-filetype=obj', optimized, '-o', split],
                        outputs=[split], stage='backend-verifier')
                    production_ir, production = unit / 'production.ll', unit / 'production.o'
                    compile_to('production-ir', production_ir, 'ir')
                    run(unit_name+'-production-verifier', [tools['opt'], '-passes=verify', '-disable-output', production_ir],
                        stage='input-verifier')
                    compile_to('production-object', production, 'object')
                    for route, obj in (('production', production), ('split', split)):
                        objects[route].append(obj)
                        run(unit_name+'-'+route+'-dwarf', [tools['llvm-dwarfdump'], '--verify', obj], stage='debug')
                    if case.get('thinlto') and optimization == 'O2':
                        bitcode = unit / 'thin.o'
                        compile_to('thinlto', bitcode, 'object', ['-flto=thin'])
                        run(unit_name+'-thin-prelink-verify', [tools['opt'], '-passes=verify', '-disable-output', bitcode],
                            stage='input-verifier')
                        objects['thinlto'].append(bitcode)
                    row['units'].append({'source': source_name, 'facts': facts, 'status': 'PASS',
                        'instructions_changed': instruction_body(base_text.read_text()) != instruction_body(opt_text.read_text()),
                        'artifacts': {str(path.relative_to(output)): digest(path) for path in unit.iterdir() if path.is_file()}})
                    executor.save()
                for route, inputs in objects.items():
                    if not inputs:
                        continue
                    binary = work / (route+'.bin')
                    link = [tools['clang++'], '--no-default-config', '--target='+target, '-fuse-ld=lld',
                        '-B'+str(stage / 'bin'), *inputs, runtime, '-Wl,--no-as-needed', objc_runtime,
                        '-Wl,--as-needed', '-pthread', '-Wl,-rpath,'+str(objc_runtime.parent),
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff', '-o', binary]
                    if route == 'thinlto':
                        link += ['-O2', '-flto=thin', '-Wl,--save-temps']
                    run(name+'-'+route+'-link', link, outputs=[binary])
                    if route == 'thinlto':
                        saved = thinlto_modules(work, inputs)
                        for index, bitcode in enumerate(saved):
                            run(name+'-thin-postlink-'+str(index), [tools['opt'], '-passes=verify', '-disable-output', bitcode],
                                stage='input-verifier')
                        row['thinlto_ir'] = {str(path.relative_to(output)): digest(path) for path in saved}
                    # Loader identity is checked before executing the real oracle.
                    graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
                        required=(objc_runtime,), run=lambda suffix, cmd, route=route:
                        run(name+'-'+route+'-'+suffix, cmd))
                    run(name+'-'+route+'-oracle', [binary], stage='runtime-oracle')
                    run(name+'-'+route+'-dwarf', [tools['llvm-dwarfdump'], '--verify', binary], stage='debug')
                    lines = run(name+'-'+route+'-line-tables', [tools['llvm-dwarfdump'], '--debug-line', binary], stage='debug')
                    if not any(source in lines for source in case['sources']):
                        raise StageFailure(name, 'dwarf-debugger', 'real fixture source missing from line tables')
                    row['executions'].append({'route': route, 'status': 'PASS', 'binary_sha256': digest(binary),
                                              'loaded_graph': graph})
                if {unit['source'] for unit in row['units']} != set(case['sources']):
                    raise StageFailure(name, 'infrastructure', 'executed units differ from selected corpus')
                row['inventory'] = verify_inventory(case, optimization, row, report['commands'])
                row['status'] = 'PASS'
                executor.save()
        changed = [row['name'] for row in report['cases'] if row['name'].endswith('-O2') and
                   any(unit['instructions_changed'] for unit in row['units'])]
        if not changed or [row['name'] for row in report['cases']] != report['expected_cases']:
            raise StageFailure('corpus-coverage', 'infrastructure', 'missing cases or skipped optimizer work')
        report['acceptance_controls'] = qualify_controls(repo, output / 'acceptance-controls', tools, environment)
        for path, signature in report['inputs'].items():
            if digest(Path(path)) != signature:
                raise StageFailure('immutable-inputs', 'infrastructure', 'qualification input changed: '+path)
        for name, tool in tool_receipt['tools'].items():
            if digest(Path(tool['path'])) != tool['sha256']:
                raise StageFailure('immutable-tools', 'infrastructure', 'qualification tool changed: '+name)
        verify_receipt(tool_receipt, plan=plan, tools=tools, repo=repo)
        report.update(status='PASS', changed_O2_cases=changed,
                      executed_cases=len(report['cases']),
                      executed_stages=[name for row in report['cases'] for name in row['inventory']['executed_stages']],
                      identity=identity(report['inputs']))
    except (OSError, ValueError, RuntimeError) as error:
        report.update(status='FAIL', error=str(error),
            classification=getattr(error, 'classification', 'infrastructure'),
            first_failing_stage=getattr(error, 'stage', None))
        raise
    finally:
        report['elapsed_seconds'] = round(time.time()-report['started_seconds'], 6)
        executor.save()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--runtime', type=Path, required=True)
    parser.add_argument('--objc-runtime', type=Path, required=True)
    parser.add_argument('--facade', type=Path, required=True)
    parser.add_argument('--timeout', type=float, default=180)
    parser.add_argument('--tools-receipt', type=Path)
    args = parser.parse_args(argv)
    if args.timeout <= 0:
        parser.error('timeout must be positive')
    result = qualify(args.repo, args.output, runtime=args.runtime, objc_runtime=args.objc_runtime,
                     facade=args.facade, timeout=args.timeout, tools_receipt=args.tools_receipt)
    print(json.dumps({'status': result['status'], 'cases': result['executed_cases'],
                      'commands': len(result['commands'])}))


if __name__ == '__main__':
    main()
