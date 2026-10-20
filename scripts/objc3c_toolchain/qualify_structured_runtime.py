"""Run the installed public task-group and actor ABI in native language modes."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
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
    modes = [('c', 'c17', 'none')]
    modes += [('objective-c', 'c17', mode) for mode in ('arc', 'mrc')]
    modes += [('objective-c++', standard, mode) for standard in ('c++20', 'c++23') for mode in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    return [(*mode, opt, False) for mode in modes for opt in ('O0', 'O2')] + [
        ('c++', standard, 'none', opt, True) for standard in ('c++20', 'c++23') for opt in ('O0', 'O2')]


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    fixture = sdk / 'share/objc3c/examples/structured-runtime'
    env = environment(sdk)
    commands, cases = [], []

    def run(label, argv, cwd):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=45)
        commands.append({'name': label, 'exit_code': result.returncode, 'log_sha256': packages.digest(log)})
        if result.returncode: raise ValueError('installed structured runtime failed: ' + label)

    try:
        with tempfile.TemporaryDirectory(prefix="objc3 structured runtime 'outside checkout ") as temporary:
            root = Path(temporary)
            shutil.copyfile(fixture / 'contract.c', root / 'contract.c')
            shutil.copyfile(fixture / 'worker.cpp', root / 'worker.cpp')
            for language, standard, ownership, optimization, threaded in configurations():
                label = '-'.join((language, standard, ownership, optimization, 'workers' if threaded else 'manual'))
                source = root / ('worker.cpp' if threaded else 'contract.c')
                obj, executable = root / 'client.o', root / 'client'
                compile_command = [sdk / 'bin/objc3c', '-x', language, source, '-std=' + standard,
                    '-' + optimization, '-g', '-UNDEBUG', '-I' + str(sdk / 'include/objc3c/runtime'), '-c', '-o', obj]
                if ownership == 'mrc': compile_command.append('-fno-objc-arc')
                elif ownership == 'arc': compile_command.append('-fobjc-arc')
                run(label + '-compile', compile_command, root)
                run(label + '-link', [sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld', obj,
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                    '-L' + str(sdk / 'lib'), '-lobjc3_runtime_services', '-lobjc', '-pthread',
                    '-Wl,-rpath,' + str(sdk / 'lib'), '-o', executable], root)
                run(label + '-run', [executable], root)
                run(label + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable], root)
                cases.append({'language': language, 'standard': standard, 'ownership': ownership,
                              'optimization': optimization, 'threaded': threaded,
                              'executable_sha256': packages.digest(executable)})
        report = {'schema': 'objc3c.structured-runtime-qualification.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'], 'executions': len(cases), 'cases': cases,
                  'commands': commands, 'source_hashes': {name: packages.digest(fixture / name) for name in ('contract.c', 'worker.cpp')}}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.structured-runtime-qualification.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'], 'cases': cases, 'commands': commands, 'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root or Path(__file__).resolve().parents[3], args.output_dir)
    print(json.dumps({'status': report['status'], 'structured_runtime_executions': report['executions'],
                      'structured_runtime_commands': len(report['commands'])}))
    return 0


if __name__ == '__main__': raise SystemExit(main())
