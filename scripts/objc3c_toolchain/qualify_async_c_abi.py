"""Qualify the installed C17/Objective-C17 and C++ typed async frame ABI."""
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
    modes = [('c', 'none', 'c++', standard, 'none')
             for standard in ('c++20', 'c++23')]
    modes += [('objective-c', provider, 'objective-c++', standard, consumer)
              for standard in ('c++20', 'c++23')
              for provider in ('arc', 'mrc') for consumer in ('arc', 'mrc')]
    return [(*mode, optimization) for mode in modes for optimization in ('O0', 'O2')]


def qualify(sdk: Path, output: Path, *, fixture: Path | None = None,
            headers: Path | None = None, runtime_library: Path | None = None,
            objc_runtime: Path | None = None, compiler: Path | None = None,
            sanitizer: str | None = None, resource_root: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    development = any(value is not None for value in
                      (fixture, headers, runtime_library, objc_runtime, compiler))
    fixture = fixture or sdk / 'share/objc3c/examples/async-c-abi'
    headers = headers or sdk / 'include/objc3c/runtime'
    runtime_library = runtime_library or sdk / 'lib/libobjc3_runtime_services.a'
    objc_runtime = objc_runtime or sdk / 'lib/libobjc.so'
    compiler = compiler or sdk / 'bin/clang'
    env = environment(sdk)
    env['LD_LIBRARY_PATH'] = str(objc_runtime.parent) + ':' + str(sdk / 'lib')
    env.update(ASAN_OPTIONS='detect_leaks=1:halt_on_error=1',
               UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1',
               TSAN_OPTIONS='halt_on_error=1')
    cases, commands = [], []

    def run(label, argv, cwd):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=120)
        commands.append({'name': label, 'exit_code': result.returncode,
                         'log_sha256': packages.digest(log)})
        if result.returncode:
            raise ValueError('typed async C ABI failed: ' + label)
        return log

    try:
        with tempfile.TemporaryDirectory(prefix="objc3 C async 'outside checkout ") as temporary:
            root = Path(temporary)
            for plang, powner, clang, standard, cowner, opt in configurations():
                label = '-'.join((plang, powner, standard, cowner, opt))
                for name in ('bridge.h', 'bridge.c', 'client.cpp'):
                    shutil.copyfile(fixture / name, root / name)
                common = ['--no-default-config', '-' + opt, '-g', '-UNDEBUG',
                          '-Wall', '-Wextra', '-Werror', '-Wno-unused-command-line-argument',
                          '-I' + str(headers), '-fobjc-runtime=gnustep-2.2', '-fblocks']
                if sanitizer:
                    common += ['-fsanitize=' + sanitizer, '-fno-omit-frame-pointer']
                    if resource_root:
                        common += ['-resource-dir', str(resource_root)]
                provider_flags = [*common, '-x', plang, '-std=c17']
                consumer_flags = [*common, '-x', clang, '-std=' + standard,
                                  '-fcoro-aligned-allocation']
                if plang == 'objective-c':
                    provider_flags += ['-fobjc-version=3',
                        '-fobjc-arc' if powner == 'arc' else '-fno-objc-arc']
                    consumer_flags += ['-fobjc-version=3',
                        '-fobjc-arc' if cowner == 'arc' else '-fno-objc-arc']
                run(label + '-provider', [compiler, *provider_flags, root / 'bridge.c',
                                         '-c', '-o', root / 'bridge.o'], root)
                (root / 'bridge.c').unlink()
                run(label + '-consumer', [compiler, *consumer_flags, root / 'client.cpp',
                                         '-c', '-o', root / 'client.o'], root)
                executable = root / 'client'
                link = [sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                        root / 'bridge.o', root / 'client.o', runtime_library,
                        '-pthread', '-L' + str(sdk / 'lib'),
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff']
                if plang == 'objective-c':
                    link += [objc_runtime, '-Wl,-rpath,' + str(objc_runtime.parent)]
                if sanitizer:
                    link += ['-fsanitize=' + sanitizer]
                    if resource_root:
                        link += ['-resource-dir', resource_root]
                run(label + '-link', [*link, '-o', executable], root)
                run(label + '-execute', [executable], root)
                run(label + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable], root)
                debugger_values = None
                if opt == 'O0' and not sanitizer:
                    debugger = [sdk / 'bin/lldb', '--batch']
                    for query in ('settings set target.disable-aslr false',
                                  'breakpoint set -n objc3_async_c_checkpoint', 'run',
                                  'expression -- (int)(observed == 42)',
                                  'frame select 1', 'expression -- (int)(value == 40)',
                                  'breakpoint disable 1', 'continue'):
                        debugger += ['-o', query]
                    log = run(label + '-lldb', [*debugger, executable], root)
                    text = log.read_text()
                    debugger_values = list(map(int, re.findall(r'\(int\) \$\d+ = (-?\d+)', text)))
                    if debugger_values != [1, 1] or 'exited with status = 0' not in text:
                        raise ValueError('mixed-language resumed debugger values missing: ' + label)
                if plang == 'c':
                    log = run(label + '-needed-libraries',
                              [sdk / 'bin/llvm-readobj', '--needed-libs', executable], root)
                    if 'libobjc' in log.read_text():
                        raise ValueError('value-only C/C++ async bridge links libobjc')
                cases.append({'provider_language': plang, 'provider_standard': 'c17',
                              'provider_ownership': powner, 'consumer_language': clang,
                              'consumer_standard': standard, 'consumer_ownership': cowner,
                              'optimization': opt, 'provider_source_removed': True,
                              'debugger_values': debugger_values,
                              'executable_sha256': packages.digest(executable)})
        report = {'schema': 'objc3c.async-c-abi-qualification.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'], 'service_abi': 4, 'async_abi': 1,
                  'runtime_binding': 'development-overlay' if development else 'sealed-sdk',
                  'compiler_sha256': packages.digest(compiler.resolve()),
                  'runtime_library_sha256': packages.digest(runtime_library),
                  'objc_runtime_sha256': packages.digest(objc_runtime),
                  'header_hashes': {name: packages.digest(headers / name)
                      for name in ('objc3_task.hpp', 'objc3_runtime_services.h')},
                  'sanitizer': sanitizer, 'executions': len(cases), 'cases': cases,
                  'commands': commands, 'source_hashes': {name: packages.digest(fixture / name)
                      for name in ('bridge.h', 'bridge.c', 'client.cpp')}}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.async-c-abi-qualification.v1',
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
    print(json.dumps({'status': report['status'], 'async_c_abi_executions': report['executions'],
                      'async_c_abi_commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
