"""Exercise only installed headers, libraries and tools outside the checkout."""
from __future__ import annotations
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


def qualify_installed(repo: Path, prefix: Path, output: Path) -> dict:
    from .check_onboarding import check
    onboarding = check(repo)
    output.mkdir(parents=True, exist_ok=True)
    env = {**os.environ, 'LD_LIBRARY_PATH': str(prefix / 'lib')}
    records = []
    with tempfile.TemporaryDirectory(prefix='objc3-installed-') as temporary:
        client = Path(temporary)
        shutil.copytree(repo / 'tests/clang_foundation/installed_sdk', client, dirs_exist_ok=True)
        def run(name, args, *, diagnostic=None):
            start = time.monotonic()
            log_path = output / f'{name}.log'
            with log_path.open('w') as log:
                result = subprocess.run([str(arg) for arg in args], cwd=client, env=env,
                                        stdout=log, stderr=subprocess.STDOUT, timeout=180)
            expected = 1 if diagnostic else 0
            records.append({'name': name, 'exit_code': result.returncode,
                            'expected_exit_code': expected,
                            'elapsed_seconds': round(time.monotonic() - start, 6)})
            if result.returncode != expected or (diagnostic and diagnostic not in log_path.read_text()):
                raise ValueError(f'installed SDK {name} failed; see {log_path}')
        compiler = prefix / 'bin/objc3c'
        run('version', [compiler, '--version'])
        run('help', [compiler, '--help'])
        language_source = 'driver-language.objc3'
        run('driver-language-c', [compiler, '--objc3', '-Werror', '-x', 'c', '-std=c17',
            '-c', language_source, '-o', 'driver-language.o'])
        run('driver-language-cxx', [compiler, '--objcxx', '-Werror', '-xc++', '-std=c++20',
            '-DEXPECT_CXX=1', '-fsyntax-only', language_source])
        shutil.copyfile(client / language_source, client / 'driver-language.m')
        run('driver-language-c-on-m', [compiler, '-Werror', '--language=c',
            '-std=c17', '-fsyntax-only', 'driver-language.m'])
        explicit_source = 'driver-language-explicit.objc3'
        (client / explicit_source).write_text(
            '#define EXPECT_CXX 1\n#include "driver-language.objc3"\n')
        run('driver-language-objc-arc', [compiler, '-Werror', '-fsyntax-only',
            '-DEXPECT_OBJC=1', '-DEXPECT_ARC=1',
            '-x', 'objective-c++', explicit_source, '-x', 'none', language_source])
        run('driver-language-objcxx-mrc', [compiler, '--objcxx', '-Werror', '-std=c++20',
            '-fno-objc-arc', '-DEXPECT_OBJC=1', '-DEXPECT_CXX=1', '-fsyntax-only', language_source])
        run('driver-language-object-link', [compiler, '--objcxx', '-Werror',
            'driver-language.o', '-o', 'driver-language-application'])
        run('driver-language-object-run', [client / 'driver-language-application'])
        run('driver-language-missing-operand', [compiler, '-x'],
            diagnostic="argument to '-x' is missing (expected 1 value)")
        run('driver-language-invalid', [compiler, '-x', 'unsupported-driver-language',
            '-fsyntax-only', language_source],
            diagnostic="language not recognized: 'unsupported-driver-language'")
        run('driver-language-explicit-objc-version', [compiler, '-xc', '-fobjc-version=3',
            '-fsyntax-only', language_source],
            diagnostic="invalid argument '-fobjc-version=3' not allowed with 'C'")
        for standard in ('c++20', 'c++23'):
            pch = client / ('driver-' + standard + '.pch')
            consumer = client / ('pch-consumer-' + standard + '.o')
            application = client / ('pch-application-' + standard)
            mode = (['-x', 'objective-c++-header'] if standard == 'c++20'
                    else ['--precompile', '-xobjective-c++-header'])
            run('pch-generate-' + standard, [compiler, *mode, '-std=' + standard,
                '-g', '-Werror', 'driver-pch.h', '-o', pch])
            run('pch-consume-' + standard, [compiler, '-std=' + standard,
                '-g', '-Werror', '-include-pch', pch, '-c', 'driver-pch.mm', '-o', consumer])
            run('pch-link-' + standard, [compiler, '-Werror', consumer, '-o', application])
            run('pch-execute-' + standard, [application])
        run('independent-compile', [compiler, '-Werror', '-std=c17', '-c', 'main.objc3', '-o', 'main.o'])
        run('independent-link', [compiler, '-Werror', 'main.o', '-o', 'independent'])
        run('independent-run', [client / 'independent'])
        run('c-helper-compile', [compiler, '-Werror', '-std=c17', '-c', 'extra.c', '-o', 'extra.o'])
        for optimize in ('O0', 'O2'):
            application = client / ('application-' + optimize)
            run(f'compile-{optimize}', [compiler, 'main.objc3', 'extra.o', f'-{optimize}', '-g',
                                      '-MMD', '-MF', 'main.d', '-o', application])
            if 'main.objc3' not in (client / 'main.d').read_text():
                raise ValueError('Clang dependency output omits the source')
            run(f'run-{optimize}', [application])
            transfer = client / ('carrier-transfer-' + optimize)
            run(f'carrier-transfer-compile-{optimize}', [compiler, 'carrier-transfer.mm',
                '-std=c++20', '-fexceptions', '-fobjc-arc-exceptions', f'-{optimize}', '-g', '-o', transfer])
            run(f'carrier-transfer-run-{optimize}', [transfer])
            for language in ('objective-c', 'objective-c++'):
                assignment = client / ('carrier-assignment-' + language + '-' + optimize)
                flags = ['-std=c++20'] if language.endswith('++') else ['-std=c17']
                mode = ['-x' + language] if optimize == 'O2' else ['-x', language]
                run(f'carrier-assignment-compile-{language}-{optimize}',
                    [compiler, *mode, 'carrier-assignment.m', *flags,
                     '-fexceptions', '-fobjc-arc-exceptions', f'-{optimize}', '-g', '-o', assignment])
                run(f'carrier-assignment-run-{language}-{optimize}', [assignment])
        (client / 'arguments.rsp').write_text('main.objc3 -O2 -o "response app"')
        run('response-file', [compiler, '@arguments.rsp'])
        run('run-response', [client / 'response app'])
        run('multiple-sources', [compiler, 'extra.c', 'multiple.c', '-o', 'multiple'])
        run('run-multiple', [client / 'multiple'])
        run('embedding-configure', ['cmake', '-S', client, '-B', client / 'build', '-G', 'Ninja',
             f'-DCMAKE_PREFIX_PATH={prefix}', f'-DCMAKE_CXX_COMPILER={prefix / "bin/clang++"}'])
        run('embedding-build', ['cmake', '--build', client / 'build', '--parallel', '2'])
        run('embedding-run', [client / 'build/embedding', prefix])
        run('embedding-dwarf', [prefix / 'bin/llvm-dwarfdump', '--verify', client / 'build/embedding'])
        run('dwarf', [prefix / 'bin/llvm-dwarfdump', '--verify', client / 'application-O0'])
        run('carrier-transfer-dwarf', [prefix / 'bin/llvm-dwarfdump', '--verify', client / 'carrier-transfer-O0'])
        for language in ('objective-c', 'objective-c++'):
            run(f'carrier-assignment-dwarf-{language}',
                [prefix / 'bin/llvm-dwarfdump', '--verify', client / ('carrier-assignment-' + language + '-O0')])
        run('debugger', [prefix / 'bin/lldb', '--batch', '-o', 'settings set target.disable-aslr false',
                        '-o', 'breakpoint set --name main', '-o', 'run',
                        '-o', 'frame variable cleanup_order', '-o', 'expression -- cleanup_order + 42',
                        '-o', 'continue', client / 'application-O0'])
        debugger = (output / 'debugger.log').read_text()
        if any(expected not in debugger for expected in
               ('cleanup_order = 0', ' = 42', 'exited with status = 0')):
            raise ValueError('installed debugger did not inspect, evaluate and run the native application')
    report = {'status': 'PASS', 'prefix': str(prefix), 'commands': records,
              'onboarding': onboarding}
    (output / 'summary.json').write_text(json.dumps(report, indent=2) + '\n')
    return report
