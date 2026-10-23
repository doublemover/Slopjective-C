"""Inspect native carriers, source bindings and resources with installed LLDB."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time

if __package__:
    from . import binary_packages as packages
    from .sdk_example import environment
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from sdk_example import environment


def _debugger_plan() -> list[tuple[str, dict]]:
    def summary(name: str, value: str, *, child: str | None = None,
                no_child: str | None = None) -> tuple[str, dict]:
        return ('frame variable ' + name,
                {'summaries': {name: value}, 'contains': [child] if child else [],
                 'absent': [no_child] if no_child else []})

    plan = [('settings set target.disable-aslr false', {})]
    for name in ('values', 'transition', 'then', 'else', 'continuation'):
        plan.append(('breakpoint set --name carrier_' + name + '_stop', {}))
    plan += [
        ('run', {}), ('up', {}),
        summary('present', 'Some(7)', child='value = 7'),
        summary('immutable', 'Some(11)', child='value = 11'),
        summary('absent', 'None', no_child='value ='),
        summary('tagged_nil', 'Some(nil)', child='value ='),
        summary('nested', 'Some(Some(9))'),
        summary('aggregate', 'Some({...})', child='first = 3'),
        summary('outcome', 'Ok(42)', child='success = 42', no_child='error ='),
        summary('zero_error', 'Err(0)', child='error = 0', no_child='success ='),
        ('frame variable unit unit_ok unit_error both_ok both_error', {
            'summaries': {'unit': 'Some(void)', 'unit_ok': 'Ok(void)',
                          'unit_error': 'Err(void)', 'both_ok': 'Ok(void)',
                          'both_error': 'Err(void)'},
            'absent': ['value =', 'success =', 'error =']}),
        ('frame variable present.value', {'contains': ['present.value = 7']}),
        ('frame variable outcome.success', {'contains': ['outcome.success = 42']}),
        ('expression -- present', {'contains': ['Some(7)', 'value = 7']}),
        ('expression -- nested', {'contains': ['Some(Some(9))']}),
        ('expression -- outcome', {'contains': ['Ok(42)', 'success = 42'],
                                    'absent': ['error =']}),
        ('expression -- present.__objc3_payload.__objc3_value + '
         'outcome.__objc3_payload.__objc3_success', {'integer': 49}),
        ('expression -- tagged_nil.__objc3_payload.__objc3_value == (id)0',
         {'boolean': True}),
        ('expression -- bound_nil == (id)0', {'boolean': True}),
        ('expression -- ((unsigned char *)&absent)[0] = 2', {}),
        summary('absent', '<invalid carrier tag 2>', no_child='value ='),
        ('expression -- ((unsigned char *)&absent)[0] = 0', {}),
        summary('absent', 'None', no_child='value ='),
        ('continue', {}), ('up', {}),
        summary('present', 'None', no_child='value ='),
        summary('tagged_nil', 'None', no_child='value ='),
        summary('nested', 'Some(None)'),
        summary('outcome', 'Err(-5)', child='error = -5', no_child='success ='),
        ('expression -- present', {'contains': ['None'], 'absent': ['value =']}),
        ('expression -- outcome', {'contains': ['Err(-5)', 'error = -5'],
                                    'absent': ['success =']}),
        ('frame variable outcome.error', {'contains': ['outcome.error = -5']}),
        ('expression -- outcome.__objc3_payload.__objc3_error', {'integer': -5}),
    ]
    # A completed if binding belongs to its then branch. Guard bindings and
    # resource locals remain visible until their real enclosing compound ends.
    for scope, events in (('then', 0), ('continuation', 0),
                          ('else', 43), ('continuation', 43)):
        expected = {'contains': ['stable = 41', 'handle = 3'], 'absent': []}
        if scope == 'then':
            expected['contains'].append('local = 42')
        else:
            expected['absent'].append('local =')
        plan += [('continue', {}), ('up', {}), ('frame variable', expected),
                 ('expression -- handle + stable', {'integer': 44}),
                 ('expression -- cleanup_events', {'integer': events})]
    plan.append(('continue', {'contains': ['exited with status = 0']}))
    return plan


def _check_transcript(text: str, plan: list[tuple[str, dict]]) -> None:
    headers = list(re.finditer(r'(?m)^\(lldb\) (.*)\n', text))
    sections = [(match.group(1), text[match.end():
                 headers[index + 1].start() if index + 1 < len(headers) else len(text)])
                for index, match in enumerate(headers)]
    cursor = 0
    for command, expected in plan:
        while cursor < len(sections) and sections[cursor][0] != command:
            cursor += 1
        if cursor == len(sections):
            raise ValueError('LLDB did not execute command: ' + command)
        output = sections[cursor][1]
        cursor += 1
        if re.search(r'(?m)^error:', output):
            raise ValueError('LLDB command failed: ' + command)
        for name, value in expected.get('summaries', {}).items():
            pattern = (r'(?m)^\([^\n]*\) ' + re.escape(name) + r' = ' +
                       re.escape(value) + r'(?:$|[\s{])')
            if not re.search(pattern, output):
                raise ValueError(f'LLDB lost native {name} summary {value}: {command}')
        for value in expected.get('contains', []):
            if value not in output:
                raise ValueError(f'LLDB output lacks {value!r}: {command}')
        for value in expected.get('absent', []):
            if re.search(r'(?<![A-Za-z0-9_])' + re.escape(value), output):
                raise ValueError(f'LLDB exposed inactive/out-of-scope {value!r}: {command}')
        if 'integer' in expected:
            values = re.findall(r'\((?:int|long)\) \$\d+ = (-?\d+)', output)
            if values != [str(expected['integer'])]:
                raise ValueError('LLDB field expression returned incorrect value: ' + command)
        if 'boolean' in expected:
            values = re.findall(r'\(bool\) \$\d+ = (true|false)', output)
            if values != ['true' if expected['boolean'] else 'false']:
                raise ValueError('LLDB nullable-payload expression failed: ' + command)
        if expected and any(name in output for name in
                            ('__objc3_state =', '__objc3_payload =',
                             '__objc3_value =', '__objc3_success =',
                             '__objc3_error =')):
            raise ValueError('LLDB displayed physical carrier fields: ' + command)


def qualify(sdk: Path, output: Path, *, compiler: Path | None = None,
            foundation: Path | None = None, fixture: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    foundation = (foundation or sdk).resolve(strict=True)
    compiler = (compiler or sdk / 'bin/objc3c').resolve(strict=True)
    fixture = (fixture or sdk / 'share/objc3c/examples/carrier-debugger').resolve(strict=True)
    packages.disjoint(output, sdk, foundation, fixture)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    env = environment(sdk)
    env['LD_LIBRARY_PATH'] = str(foundation / 'lib') + os.pathsep + str(sdk / 'lib')
    env['TERM'] = 'dumb'
    report = {'schema': 'objc3c.installed-carrier-debugger.v1', 'status': 'running',
              'runtime_sdk_seal': manifest['seal_sha256'], 'commands': [], 'cases': [],
              'compiler': str(compiler), 'foundation': str(foundation),
              'profile_conformance_claimed': False}

    def persist() -> None:
        packages.write_json(receipt, report)

    def run(label: str, argv, cwd: Path) -> str:
        log = output / (label + '.log')
        started = time.monotonic()
        timed_out = False
        with log.open('w') as stream:
            process = subprocess.Popen(list(map(str, argv)), cwd=cwd, env=env,
                                       stdout=stream, stderr=subprocess.STDOUT,
                                       start_new_session=True)
            try:
                code = process.wait(timeout=120)
            except subprocess.TimeoutExpired:
                timed_out = True
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                code = process.wait()
        report['commands'].append({'name': label, 'exit_code': code,
                                   'command': list(map(str, argv)),
                                   'timed_out': timed_out,
                                   'elapsed_seconds': round(time.monotonic() - started, 6),
                                   'log_sha256': packages.digest(log)})
        persist()
        if code or timed_out:
            raise ValueError('native carrier debugger failed: ' + label + '; see ' + str(log))
        return log.read_text()

    persist()
    try:
        inputs = [compiler, sdk / 'bin/clang', foundation / 'bin/lldb',
                  foundation / 'bin/clang++',
                  foundation / 'bin/llvm-dwarfdump', sdk / 'lib/libobjc.so',
                  sdk / 'objc3-sdk.json', fixture / 'values.h', fixture / 'main.m',
                  Path(__file__).resolve()]
        inputs += sorted(path for path in (foundation / 'lib').glob('liblldb*.so*')
                         if path.is_file())
        hashes = {str(path): packages.digest(path) for path in inputs}
        report['input_sha256'] = hashes
        plan = _debugger_plan()
        with tempfile.TemporaryDirectory(prefix="objc3 native carriers 'outside checkout ") as temporary:
            root = Path(temporary)
            for language, standard in (('objective-c', 'c17'),
                                       ('objective-c++', 'c++20'),
                                       ('objective-c++', 'c++23')):
                for arc in (False, True):
                    label = standard + ('-arc' if arc else '-mrc')
                    directory = root / label
                    directory.mkdir()
                    main = directory / ('main.mm' if '++' in language else 'main.m')
                    shutil.copyfile(fixture / 'main.m', main)
                    shutil.copyfile(fixture / 'values.h', directory / 'values.h')
                    obj, executable = directory / 'main.o', directory / 'application'
                    flags = ['-fobjc-arc', '-fobjc-arc-exceptions'] if arc else []
                    run(label + '-compile', [compiler, '-x', language,
                        '-std=' + standard, '-fobjc-runtime=gnustep-2.2',
                        '-fexceptions', '-fblocks', '-O0', '-g', '-fstandalone-debug',
                        '-Wno-nullability-completeness', *flags,
                        '-isystem', sdk / 'include', '-c', main, '-o', obj], directory)
                    run(label + '-link', [foundation / 'bin/clang++',
                        '--no-default-config', '-fuse-ld=lld', obj, '-L' + str(sdk / 'lib'),
                        '-lobjc', '-Wl,-rpath,' + str(sdk / 'lib'), '-o', executable], directory)
                    run(label + '-execute', [executable], directory)
                    run(label + '-dwarf', [foundation / 'bin/llvm-dwarfdump',
                        '--verify', executable], directory)
                    argv = [foundation / 'bin/lldb', '--batch', '--no-lldbinit']
                    for command, _ in plan:
                        argv += ['-o', command]
                    text = run(label + '-lldb', [*argv, executable], directory)
                    _check_transcript(text, plan)
                    report['cases'].append({
                        'language': language, 'standard': standard,
                        'ownership': 'arc' if arc else 'mrc', 'optimization': 'O0',
                        'selected_payloads': True, 'inactive_payload_hidden': True,
                        'some_nil_preserved': True, 'void_arms_preserved': True,
                        'nested_payloads': True, 'arm_transitions_refetched': True,
                        'same_stop_tag_writes_refetched': True,
                        'invalid_tag_payload_hidden': True,
                        'guard_and_if_scopes_preserved': True,
                        'resource_source_local_preserved': True,
                        'native_field_expressions': True,
                        'native_expression_results_formatted': True,
                        'continued_exit_code': 0,
                        'executable_sha256': packages.digest(executable)})
                    persist()
        if hashes != {str(path): packages.digest(path) for path in inputs}:
            raise ValueError('native debugger inputs changed during qualification')
        report.update(status='PASS', executions=len(report['cases']),
                      commands_passed=len(report['commands']),
                      temporary_artifacts_reclaimed=True)
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        persist()
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.output)
    print(f"{result['status']}: {result['executions']} native carrier debugger cases")


if __name__ == '__main__':
    main()
