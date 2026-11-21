"""Check optimized native carrier/cleanup/coroutine state with matched LLDB."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import shutil

from .binary_packages import digest, write_json
from .llvm_ir_execution import Execution
from .llvm_tools import selected, verify_receipt
from .llvm_debugger import plan as debugger_plan
from .qualify_carrier_debugger import _check_transcript


def check_optimized_away(output: str):
    """Unavailable is valid; a visible wrong value is not."""
    values = re.findall(r'(?m)^\(int\) optimized_away\s*=\s*([^\n]+)$', output)
    if len(values) != 1:
        raise ValueError('optimized debugger displayed an incorrect or unclassified local value')
    if re.fullmatch(r'<(?:variable not available|optimized out|no location, value may have been optimized out|register [A-Za-z0-9]+ is not available)>', values[0]):
        return 'unavailable'
    if values[0] == '17':
        return 'correct-visible'
    raise ValueError('optimized debugger displayed an incorrect or unclassified local value')


def check_source_frame(output: str, name: str):
    if not re.search(r'frame #\d+:.*\bat [^\n]*'+re.escape(name)+r':\d+', output):
        raise ValueError('optimized debugger lost the expected real source location: '+name)


def transcript_sections(text: str) -> dict[str, list[str]]:
    matches = list(re.finditer(r'(?m)^\(lldb\) (.*)\n', text))
    sections = {}
    for index, match in enumerate(matches):
        sections.setdefault(match[1], []).append(text[match.end():matches[index+1].start()
            if index+1 < len(matches) else len(text)])
    return sections


def carrier_plan():
    return [
        ('settings set target.disable-aslr false', {}),
        ('breakpoint set -n objc3_optimized_debug_checkpoint', {}),
        ('run', {}),
        ('expression -- (int)(phase == 1 && observed == 42 && storage != 0)', {'integer': 1}),
        ('up', {}),
        ('frame variable number', {'summaries': {'number': 'Some(42)'}, 'contains': ['value = 42']}),
        ('frame variable optimized_away', {}),
        ('continue', {}),
        ('expression -- (int)(phase == 2 && observed == 0 && storage != 0)', {'integer': 1}),
        ('up', {}),
        ('frame variable number', {'summaries': {'number': 'None'}, 'absent': ['value =']}),
        ('continue', {}),
        ('expression -- (int)(phase == 3 && observed == 21 && storage != 0)', {'integer': 1}),
        ('up', {}),
        ('frame variable number', {'summaries': {'number': 'None'}, 'absent': ['value =']}),
        ('continue', {'contains': ['exited with status = 0']}),
    ]


def qualify(repo: Path, output: Path, *, debugger_receipt: Path, ir_receipt: Path) -> dict:
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('optimized debugger qualification needs fresh repository out evidence')
    debugger = json.loads(debugger_receipt.read_text())
    ir = json.loads(ir_receipt.read_text())
    foundation, tools, environment = selected(repo)
    expected_debugger = debugger_plan(repo)
    if (debugger.get('status') != 'PASS' or ir.get('status') != 'PASS' or
            debugger['configuration']['identity'] != expected_debugger['identity'] or
            debugger['configuration']['binding'] != expected_debugger['binding'] or
            debugger.get('foundation_tools_identity') != ir.get('tool_identity')):
        raise ValueError('optimized debugger requires complete matched debugger and IR receipts')
    tool_receipt = json.loads((ir_receipt.parent / 'tools.json').read_text())
    verify_receipt(tool_receipt, plan=foundation, tools=tools, repo=repo)
    if (ir.get('tool_identity') != tool_receipt['identity'] or
            not ir.get('expected_cases') or
            len(ir.get('cases', [])) != len(ir['expected_cases']) or
            {row['name'] for row in ir['cases'] if row.get('status') == 'PASS'} != set(ir['expected_cases'])):
        raise ValueError('optimized debugger requires the complete qualified IR matrix')
    for path, signature in ir['inputs'].items():
        if digest(Path(path)) != signature:
            raise ValueError('qualified IR input changed: '+path)
    lldb = Path(debugger['tools']['lldb']['path'])
    for tool in debugger['tools'].values():
        if digest(Path(tool['path'])) != tool['sha256']:
            raise ValueError('debugger executable changed after qualification')
        for path, signature in tool['loaded_graph']['libraries'].items():
            if digest(Path(path)) != signature:
                raise ValueError('debugger loaded library changed after qualification')
    output.mkdir(parents=True)
    report = {'schema': 'objc3c.llvm-optimized-debug.v1', 'status': 'running', 'cases': [],
              'expected_cases': ['O2', 'O2-thinlto', 'async-production', 'async-thinlto'],
              'inputs': {str(path): digest(path) for path in (Path(__file__), debugger_receipt, ir_receipt,
                         Path(_check_transcript.__code__.co_filename),
                         ir_receipt.parent / 'tools.json',
                         repo / 'tests/clang_foundation/optimized_debugger/main.mm')}}
    for tool in debugger['tools'].values():
        report['inputs'][tool['path']] = tool['sha256']
        report['inputs'].update(tool['loaded_graph']['libraries'])
    executor = Execution(output, environment, report)
    run = executor.run
    try:
        runtime = Path(ir['runtime']['objc_runtime'])
        flags = ['--no-default-config', '--target='+ir['target'], '-x', 'objective-c++', '-std=c++20',
            '-O2', '-g', '-UNDEBUG', '-fexceptions', '-fobjc-exceptions', '-fobjc-arc-exceptions',
            '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fobjc-arc', '-fblocks']
        fixture = output / 'main.mm'
        shutil.copyfile(repo / 'tests/clang_foundation/optimized_debugger/main.mm', fixture)
        for route in ('O2', 'O2-thinlto'):
            binary = output / (route+'.bin')
            obj = output / (route+'.o')
            run(route+'-compile', [tools['clang++'], *flags, *(['-flto=thin'] if route.endswith('thinlto') else []),
                '-c', fixture, '-o', obj], outputs=[obj])
            run(route+'-link', [tools['clang++'], '--no-default-config', '-O2',
                *(['-flto=thin'] if route.endswith('thinlto') else []),
                '-fuse-ld=lld', '-B'+str(tools['ld.lld'].parent), obj, runtime,
                '-Wl,-rpath,'+str(runtime.parent), '-o', binary], outputs=[binary])
            run(route+'-execute', [binary], stage='runtime-oracle')
            run(route+'-dwarf', [tools['llvm-dwarfdump'], '--verify', binary], stage='debug')
            plan = carrier_plan()
            argv = [lldb, '--batch', '--no-lldbinit']
            for command, _ in plan:
                argv += ['-o', command]
            text = run(route+'-lldb', [*argv, binary], stage='debug')
            _check_transcript(text, plan)
            sections = transcript_sections(text)
            for section in sections['up']:
                check_source_frame(section, 'main.mm')
            state = check_optimized_away(sections['frame variable optimized_away'][0])
            # Independent controls reject a stale visible value and wrong source
            # expectation; permitted optimized-out state is accepted explicitly.
            try:
                check_optimized_away('(int) optimized_away = 999\n')
            except ValueError:
                pass
            else:
                raise ValueError('wrong visible debugger value passed')
            try:
                check_source_frame(sections['up'][0], 'incorrect-source.mm')
            except ValueError:
                pass
            else:
                raise ValueError('wrong debugger source expectation passed')
            report['cases'].append({'name': route, 'status': 'PASS', 'optimized_local': state,
                'selected_carrier_state': True, 'inactive_payload_hidden': True, 'cleanup_events': 21,
                'binary_sha256': digest(binary)})
        # Reuse genuine optimized coroutine binaries and their runtime oracle.
        async_case = next(row for row in ir['cases'] if row['name'] == 'native-async-O2')
        for route in ('production', 'thinlto'):
            record = next(row for row in async_case['executions'] if row['route'] == route)
            binary = ir_receipt.parent / 'native-async-O2' / (route+'.bin')
            if digest(binary) != record['binary_sha256']:
                raise ValueError('qualified optimized coroutine binary changed')
            plan = [('settings set target.disable-aslr false', {}),
                    ('breakpoint set -n objc3_native_async_checkpoint', {}), ('run', {}),
                    ("expression -- (int)(value == 42 && text[0] == 'p')", {'integer': 1}),
                    ('frame info', {}), ('breakpoint disable 1', {}),
                    ('continue', {'contains': ['exited with status = 0']})]
            argv = [lldb, '--batch', '--no-lldbinit']
            for command, _ in plan:
                argv += ['-o', command]
            text = run('async-'+route+'-lldb', [*argv, binary], stage='debug')
            _check_transcript(text, plan)
            check_source_frame(transcript_sections(text)['frame info'][0], 'provider.cpp')
            report['cases'].append({'name': 'async-'+route, 'status': 'PASS', 'checkpoint_value': 42,
                                    'nontrivial_frame_text': 'p', 'binary_sha256': digest(binary)})
        for path, signature in report['inputs'].items():
            if digest(Path(path)) != signature:
                raise ValueError('optimized debugger qualification input changed')
        if {row['name'] for row in report['cases']} != set(report['expected_cases']):
            raise ValueError('optimized debugger execution matrix is incomplete')
        report['status'] = 'PASS'
    except (OSError, ValueError) as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        executor.save()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--debugger-receipt', type=Path, required=True)
    parser.add_argument('--ir-receipt', type=Path, required=True)
    args = parser.parse_args(argv)
    report = qualify(args.repo, args.output, debugger_receipt=args.debugger_receipt, ir_receipt=args.ir_receipt)
    print(json.dumps({'status': report['status'], 'cases': len(report['cases'])}))


if __name__ == '__main__':
    main()
