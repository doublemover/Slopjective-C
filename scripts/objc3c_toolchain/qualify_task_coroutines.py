"""Run source-free native coroutine providers through the installed task ABI."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

if __package__:
    from . import binary_packages as packages
    from .sdk_example import environment
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from sdk_example import environment


def configurations():
    modes = [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    modes += [('objective-c++', standard, ownership)
              for standard in ('c++20', 'c++23') for ownership in ('arc', 'mrc')]
    return [(*mode, optimization) for mode in modes for optimization in ('O0', 'O2')]


def qualify(sdk: Path, output: Path, *, fixture: Path | None = None,
            headers: Path | None = None, runtime_library: Path | None = None,
            objc_runtime: Path | None = None, compiler: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    development = any(value is not None for value in (fixture, headers, runtime_library, objc_runtime, compiler))
    fixture = fixture or sdk / 'share/objc3c/examples/task-coroutines'
    headers = headers or sdk / 'include/objc3c/runtime'
    runtime_library = runtime_library or sdk / 'lib/libobjc3_runtime_services.a'
    objc_runtime = objc_runtime or sdk / 'lib/libobjc.so'
    native_compiler = compiler or sdk / "bin/clang"
    compiler = compiler or sdk / "bin/objc3c"
    env = environment(sdk)
    env['LD_LIBRARY_PATH'] = str(objc_runtime.parent) + ':' + str(sdk / 'lib')
    commands, cases = [], []

    def run(label, argv, cwd):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=90)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': packages.digest(log)})
        if result.returncode:
            raise ValueError('installed task coroutines failed: ' + label)
        return log

    try:
        with tempfile.TemporaryDirectory(prefix="objc3 coroutines 'outside checkout ") as temporary:
            root = Path(temporary)
            for language, standard, ownership, optimization in configurations():
                label = '-'.join((language, standard, ownership, optimization))
                for name in ('provider.cpp', 'provider.h', 'client.cpp'):
                    shutil.copyfile(fixture / name, root / name)
                flags = ['-x', language, '-std=' + standard, '-' + optimization, '-g',
                         '-UNDEBUG', '-Wall', '-Wextra', '-Werror',
                         '-Wno-unused-command-line-argument', '-fcoro-aligned-allocation',
                         '-fobjc-runtime=gnustep-2.2', '-fblocks',
                         '-I' + str(headers)]
                if language == 'objective-c++': flags.append('-fobjc-version=3')
                if ownership == 'arc': flags.append('-fobjc-arc')
                elif ownership == 'mrc': flags.append('-fno-objc-arc')
                run(label + '-provider', [compiler, *flags, root / 'provider.cpp',
                    '-c', '-o', root / 'provider.o'], root)
                # The consumer must build and execute without implementation
                # source; only its header and the native provider object remain.
                (root / 'provider.cpp').unlink()
                run(label + '-consumer', [compiler, *flags, root / 'client.cpp',
                    '-c', '-o', root / 'client.o'], root)
                executable = root / 'client'
                run(label + '-link', [sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                    root / 'provider.o', root / 'client.o', '-L' + str(sdk / 'lib'),
                    runtime_library, objc_runtime, '-pthread',
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                    '-Wl,-rpath,' + str(objc_runtime.parent), '-o', executable], root)
                run(label + '-execute', [executable], root)
                run(label + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable], root)
                debugger_values = None
                if optimization == 'O0':
                    debugger = [sdk / 'bin/lldb', '--batch']
                    for query in ('settings set target.disable-aslr false',
                            'breakpoint set -n objc3_coroutine_debug_checkpoint', 'run',
                            "expression -- (int)(value == 42 && text[0] == 'p')",
                            'frame select 1', 'expression -- (int)(value == 0)',
                            'breakpoint disable 1', 'continue'):
                        debugger += ['-o', query]
                    log = run(label + '-lldb', [*debugger, executable], root)
                    text = log.read_text()
                    debugger_values = list(map(int, re.findall(r'\(int\) \$\d+ = (-?\d+)', text)))
                    if debugger_values != [1, 1] or 'exited with status = 0' not in text:
                        raise ValueError('native coroutine debugger lost resumed frame values: ' + label)
                cases.append({'language': language, 'standard': standard, 'ownership': ownership,
                              'optimization': optimization, 'provider_source_removed': True,
                              'debugger_values': debugger_values,
                              'executable_sha256': packages.digest(executable)})
        report = {'schema': 'objc3c.task-coroutine-qualification.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'],
                  'compiler_sha256': packages.digest(native_compiler.resolve()),
                  'runtime_binding': 'development-overlay' if development else 'sealed-sdk',
                  'runtime_library_sha256': packages.digest(runtime_library),
                  'objc_runtime_sha256': packages.digest(objc_runtime),
                  'header_hashes': {name: packages.digest(headers / name)
                      for name in ('objc3_task.hpp', 'objc3_runtime_services.h')},
                  'executions': len(cases), 'cases': cases,
                  'commands': commands, 'source_hashes': {name: packages.digest(fixture / name)
                      for name in ('provider.cpp', 'provider.h', 'client.cpp')}}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.task-coroutine-qualification.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'], 'cases': cases,
            'commands': commands, 'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root or Path(__file__).resolve().parents[3], args.output_dir)
    print(json.dumps({'status': report['status'], 'task_coroutine_executions': report['executions'],
                      'task_coroutine_commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
