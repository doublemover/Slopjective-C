"""Resolve duplicate private names using the stopped provider's native binding."""
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
    fixture = (fixture or sdk / 'share/objc3c/examples/module-names').resolve(strict=True)
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
        if result.returncode: raise ValueError('private-name debugger failed: ' + label)
        return log

    try:
        with tempfile.TemporaryDirectory(prefix="objc3 private debugger 'outside checkout ") as temporary:
            root = Path(temporary)
            for language, standard in [('objective-c', 'c17'), ('objective-c++', 'c++20'), ('objective-c++', 'c++23')]:
                for ownership in ('arc', 'mrc'):
                    label = '-'.join((language, standard, ownership))
                    directory = root / label
                    shutil.copytree(fixture, directory)
                    suffix = '.mm' if language.endswith('++') else '.m'
                    common = ['--objc3', '--runtime', 'gnustep-2.2', '--target', manifest['target']]
                    if language.endswith('++'): common += ['--objcxx']
                    if ownership == 'arc': common += ['--arc']
                    flags = ['--', '--no-default-config', '-std=' + standard, '-fblocks', '-O0', '-g',
                             '-fstandalone-debug', '-isystem', sdk / 'include']
                    imports, objects = [], []
                    for side in ('left', 'right'):
                        provider = directory / side
                        source = provider / ('module' + suffix)
                        if suffix != '.m': (provider / 'module.m').rename(source)
                        implementation = 'int debugger_marker(void) { return shared_global; }'
                        provider_text = source.read_text()
                        if provider_text.count(implementation) != 1:
                            raise ValueError('private debugger marker fixture is missing')
                        source.write_text(provider_text.replace(implementation, 'int debugger_marker(void);'))
                        secondary = provider / ('secondary' + suffix)
                        secondary.write_text('extern int shared_global;\n' + implementation + '\n')
                        name, identity = 'fixture.Names' + side.title(), 'names-' + side + '-v1'
                        interface, obj = provider / (name + '.objc3i'), provider / 'module.o'
                        objects.append(obj)
                        run(label + '-' + side, [compiler, source, *common, '--module-owned-names',
                            '--module-source-root', provider, '--module', name, '--package', identity,
                            '--emit-interface', interface, '--export-declaration', side + '_value',
                            '--emit-object', obj, *flags], directory)
                        secondary_object = provider / 'secondary.o'
                        objects.append(secondary_object)
                        run(label + '-' + side + '-secondary', [compiler, secondary, *common,
                            '--module-owned-names', '--module-source-root', provider,
                            '--module', name, '--package', identity, '--emit-object', secondary_object,
                            *flags], directory)
                        module = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                        imports += ['--module-search-path', provider, '--import-module', name,
                                    module['semantic_fingerprint'], identity]
                    source = directory / 'app' / ('main' + suffix)
                    if suffix != '.m': (directory / 'app/main.m').rename(source)
                    obj = directory / 'main.o'
                    objects.append(obj)
                    run(label + '-client', [compiler, source, *common, '--source-imports', *imports,
                        '--emit-object', obj, *flags], directory)
                    executable = directory / 'application'
                    run(label + '-link', [foundation / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                        *objects, '-L' + str(sdk / 'lib'), '-lobjc', '-Wl,-rpath,' + str(sdk / 'lib'),
                        '-o', executable], directory)
                    run(label + '-execute', [executable], directory)
                    run(label + '-dwarf', [foundation / 'bin/llvm-dwarfdump', '--verify', executable], directory)
                    for frame_kind in ('function', 'method'):
                        queries = ['settings set target.disable-aslr false']
                        pattern = '@try' if frame_kind == 'function' else '^[+].*shared_global'
                        for side in ('left', 'right'):
                            queries += ['breakpoint set --file "' + str(directory / side / ('module' + suffix)) +
                                        '" --source-pattern-regexp "' + pattern + '"']
                        queries += ['run']
                        expected = []
                        variable = 'object' if frame_kind == 'function' else 'self'
                        for index, (value, bias) in enumerate(((20, 1), (41, 2)), 1):
                            if frame_kind == 'method':
                                queries += ['breakpoint disable ' + str(index)]
                            queries += ['frame variable ' + variable, 'expression -- shared_global',
                                        'expression -- shared_helper()', 'expression -- debugger_marker()']
                            expected += [value, 1, value]
                            if language.endswith('++'):
                                queries += ['expression -- private_tls']
                                expected += [bias]
                            queries += ['expression -- (int)[RuntimeTwin value]',
                                        'expression -- (int)[RuntimeTwin directValue]',
                                        'expression -- (int)[RuntimeChild childValue]',
                                        'expression -- (int)[RuntimeTwin categoryValue]']
                            expected += [value + 1] * 4
                            if frame_kind == 'function':
                                queries += ['expression -- object->slot']
                                expected += [42]
                            queries += ['continue']
                        debugger = [foundation / 'bin/lldb', '--batch']
                        for query in queries: debugger += ['-o', query]
                        log = run(label + '-' + frame_kind + '-lldb', [*debugger, executable], directory)
                        text = log.read_text()
                        values = list(map(int, re.findall(r'\(int\) \$\d+ = (-?\d+)', text)))
                        source_type = '(RuntimeTwin *) object =' if frame_kind == 'function' else 'Class) self ='
                        if (values != expected or text.count(source_type) != 2 or
                                'exited with status = 0' not in text):
                            raise ValueError('private-name debugger selected another provider or lost its source type: ' + label + '-' + frame_kind)
                        cases.append({'language': language, 'standard': standard, 'ownership': ownership,
                            'frame_kind': frame_kind, 'values': values, 'source_types_preserved': True,
                            'continued_exit_code': 0, 'executable_sha256': packages.digest(executable),
                            'log_sha256': packages.digest(log)})
        report = {'schema': 'objc3c.module-names-collision-debugger.v1', 'status': 'PASS',
                  'runtime_sdk_seal': manifest['seal_sha256'], 'compiler': str(compiler),
                  'foundation': str(foundation), 'executions': len(cases), 'cases': cases,
                  'commands': commands, 'source_hashes': {str(path.relative_to(fixture)): packages.digest(path)
                      for path in sorted(fixture.rglob('*')) if path.is_file()}}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.module-names-collision-debugger.v1', 'status': 'FAIL',
            'runtime_sdk_seal': manifest['seal_sha256'], 'cases': cases, 'commands': commands, 'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report
