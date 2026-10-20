"""Verify owned Objective-C types, messages and direct calls in installed LLDB."""
from __future__ import annotations

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

if __package__:
    from . import binary_packages as packages
    from .sdk_example import environment
else:
    import binary_packages as packages
    from sdk_example import environment


def qualify(sdk: Path, output: Path, *, compiler: Path | None = None,
            foundation: Path | None = None, fixture: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    compiler = (compiler or sdk / 'bin/objc3c').resolve(strict=True)
    foundation = (foundation or sdk).resolve(strict=True)
    fixture = (fixture or sdk / 'share/objc3c/examples/module-names/debugger').resolve(strict=True)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    env = environment(sdk)
    env['LD_LIBRARY_PATH'] = str(foundation / 'lib') + os.pathsep + str(sdk / 'lib')
    commands, cases = [], []

    def run(label, argv, cwd):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=90)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': packages.digest(log)})
        if result.returncode: raise ValueError('owned-name debugger failed: ' + label)
        return log

    try:
        with tempfile.TemporaryDirectory(prefix="objc3 native debugger 'outside checkout ") as temporary:
            root = Path(temporary)
            for language, standard in [('objective-c', 'c17'), ('objective-c++', 'c++20'), ('objective-c++', 'c++23')]:
                for ownership in ('arc', 'mrc'):
                    label = '-'.join((language, standard, ownership))
                    directory = root / label
                    directory.mkdir()
                    suffix = '.mm' if language.endswith('++') else '.m'
                    provider, main = directory / ('provider' + suffix), directory / ('main' + suffix)
                    shutil.copyfile(fixture / 'provider.m', provider)
                    shutil.copyfile(fixture / 'main.m', main)
                    interface = directory / 'Names.Debugger.objc3i'
                    common = ['--objc3', '--runtime', 'gnustep-2.2', '--target', manifest['target']]
                    if language.endswith('++'): common += ['--objcxx']
                    if ownership == 'arc': common += ['--arc']
                    flags = ['--', '--no-default-config', '-std=' + standard, '-fblocks', '-O0', '-g',
                             '-fstandalone-debug', '-isystem', sdk / 'include']
                    run(label + '-provider', [compiler, provider, *common, '--module-owned-names',
                        '--module', 'Names.Debugger', '--package', 'debugger-names-v1',
                        '--emit-object', directory / 'provider.o', '--emit-interface', interface,
                        '--export-declaration', 'names_debugger_value', *flags], directory)
                    identity = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                    run(label + '-client', [compiler, main, *common, '--source-imports',
                        '--module-search-path', directory, '--import-module', 'Names.Debugger',
                        identity['semantic_fingerprint'], 'debugger-names-v1',
                        '--emit-object', directory / 'main.o', *flags], directory)
                    executable = directory / 'application'
                    run(label + '-link', [foundation / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                        directory / 'provider.o', directory / 'main.o', '-L' + str(sdk / 'lib'), '-lobjc',
                        '-Wl,-rpath,' + str(sdk / 'lib'), '-o', executable], directory)
                    run(label + '-execute', [executable], directory)
                    run(label + '-dwarf', [foundation / 'bin/llvm-dwarfdump', '--verify', executable], directory)
                    debugger = [foundation / 'bin/lldb', '--batch']
                    queries = ['settings set target.disable-aslr false',
                        'breakpoint set --file ' + provider.name + " --source-pattern-regexp 'observed = object->slot'",
                        'run', 'frame variable object', 'expression -- object->slot',
                        'expression -- (int)[object value]', 'expression -- (int)[NamesDebuggerRoot tag]',
                        'expression -- (int)[object directValue]', 'expression -- (int)[NamesDebuggerRoot directTag]',
                        'expression -- (int)[object categoryDirect]',
                        'expression -- names_debugger_invoke(object, @selector(value))',
                        'expression -- ({ SEL values[] = { @selector(value), @selector(value) }; names_debugger_invoke(object, values[1]); })',
                        'continue']
                    for query in queries: debugger += ['-o', query]
                    log = run(label + '-lldb', [*debugger, executable], directory)
                    text = log.read_text()
                    values = list(map(int, re.findall(r'\(int\) \$\d+ = (-?\d+)', text)))
                    if (values != [42, 42, 42, 43, 43, 44, 42, 42] or 'NamesDebuggerRoot *) object =' not in text or
                            'exited with status = 0' not in text):
                        raise ValueError('owned-name debugger returned incorrect values or lost its source type: ' + label)
                    cases.append({'language': language, 'standard': standard, 'ownership': ownership,
                        'values': values, 'source_type_preserved': True, 'continued_exit_code': 0,
                        'executable_sha256': packages.digest(executable), 'log_sha256': packages.digest(log)})
        report = {'schema': 'objc3c.module-names-debugger.v1', 'status': 'PASS',
                  'runtime_sdk_seal': manifest['seal_sha256'], 'compiler': str(compiler),
                  'foundation': str(foundation), 'executions': len(cases), 'cases': cases,
                  'commands': commands, 'source_hashes': {name: packages.digest(fixture / name) for name in ('provider.m', 'main.m')}}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.module-names-debugger.v1', 'status': 'FAIL',
            'runtime_sdk_seal': manifest['seal_sha256'], 'cases': cases, 'commands': commands, 'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report
