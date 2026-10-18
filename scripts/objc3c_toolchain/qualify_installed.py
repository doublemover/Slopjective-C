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
    output.mkdir(parents=True, exist_ok=True)
    env = {**os.environ, 'LD_LIBRARY_PATH': str(prefix / 'lib')}
    records = []
    with tempfile.TemporaryDirectory(prefix='objc3-installed-') as temporary:
        client = Path(temporary)
        shutil.copytree(repo / 'tests/clang_foundation/installed_sdk', client, dirs_exist_ok=True)
        def run(name, args):
            start = time.monotonic()
            with (output / f'{name}.log').open('w') as log:
                result = subprocess.run([str(arg) for arg in args], cwd=client, env=env,
                                        stdout=log, stderr=subprocess.STDOUT, timeout=180)
            records.append({'name': name, 'exit_code': result.returncode,
                            'elapsed_seconds': round(time.monotonic() - start, 6)})
            if result.returncode:
                raise ValueError(f'installed SDK {name} failed; see {output / (name + ".log")}')
        compiler = prefix / 'bin/objc3c'
        run('version', [compiler, '--version'])
        run('help', [compiler, '--help'])
        for optimize in ('O0', 'O2'):
            application = client / ('application-' + optimize)
            run(f'compile-{optimize}', [compiler, 'main.objc3', f'-{optimize}', '-g',
                                      '-MMD', '-MF', 'main.d', '-o', application])
            if 'main.objc3' not in (client / 'main.d').read_text():
                raise ValueError('Clang dependency output omits the source')
            run(f'run-{optimize}', [application])
        (client / 'arguments.rsp').write_text('main.objc3 -O2 -o "response app"')
        run('response-file', [compiler, '@arguments.rsp'])
        run('run-response', [client / 'response app'])
        run('multiple-sources', [compiler, 'extra.c', 'multiple.c', '-o', 'multiple'])
        run('run-multiple', [client / 'multiple'])
        run('embedding-configure', ['cmake', '-S', client, '-B', client / 'build', '-G', 'Ninja',
             f'-DCMAKE_PREFIX_PATH={prefix}', f'-DCMAKE_CXX_COMPILER={prefix / "bin/clang++"}',
             '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld'])
        run('embedding-build', ['cmake', '--build', client / 'build', '--parallel', '2'])
        run('embedding-run', [client / 'build/embedding', prefix])
        run('dwarf', [prefix / 'bin/llvm-dwarfdump', '--verify', client / 'application-O0'])
        run('debugger', [prefix / 'bin/lldb', '--batch', '-o', 'settings set target.disable-aslr false',
                        '-o', 'breakpoint set --name main', '-o', 'run',
                        '-o', 'frame variable cleanup_order', '-o', 'expression -- cleanup_order + 42',
                        '-o', 'continue', client / 'application-O0'])
        debugger = (output / 'debugger.log').read_text()
        if any(expected not in debugger for expected in
               ('cleanup_order = 0', ' = 42', 'exited with status = 0')):
            raise ValueError('installed debugger did not inspect, evaluate and run the native application')
    report = {'status': 'PASS', 'prefix': str(prefix), 'commands': records}
    (output / 'summary.json').write_text(json.dumps(report, indent=2) + '\n')
    return report
