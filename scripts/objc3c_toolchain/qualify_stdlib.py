"""Execute the installed native Objective-C 3.0 standard-library contracts."""
from __future__ import annotations

import argparse
import itertools
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


def qualify(sdk: Path, output: Path, *, include_root: Path | None = None,
            fixture: Path | None = None, compiler: Path | None = None,
            runtime_library: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    development = any(x is not None for x in (include_root, fixture, compiler, runtime_library))
    include_root = include_root or sdk / 'include'
    fixture = fixture or sdk / 'share/objc3c/examples/stdlib'
    compiler = compiler or sdk / 'bin/clang'
    runtime_library = runtime_library or sdk / 'lib/libobjc3_runtime_services.a'
    env = environment(sdk)
    cases, commands = [], []

    def inputs():
        return {
            'compiler_sha256': packages.digest(compiler.resolve()),
            'runtime_library_sha256': packages.digest(runtime_library),
            'source_hashes': {name: packages.digest(fixture / name)
                              for name in ('provider.h', 'provider.m', 'client.m')},
            'include_hashes': {path.relative_to(include_root).as_posix(): packages.digest(path)
                              for directory in ('objc3', 'objc3c/runtime')
                              for path in sorted((include_root / directory).rglob('*'))
                              if path.is_file()}}


    def run(label, argv, root, *, failure=None):
        log = output / (label + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=root, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=120)
        commands.append({'name': label, 'exit_code': result.returncode,
                         'expected_failure': failure is not None,
                         'log_sha256': packages.digest(log)})
        if failure is None and result.returncode != 0:
            raise ValueError('standard library failed: ' + label)
        if failure is not None and (result.returncode == 0 or failure not in log.read_text()):
            raise ValueError('standard library rejection missing: ' + label)

    try:
        initial_inputs = inputs()
        with tempfile.TemporaryDirectory(prefix="objc3 stdlib 'external ") as temporary:
            root = Path(temporary)
            modes = [('objective-c', 'c17'), ('objective-c++', 'c++20'),
                     ('objective-c++', 'c++23')]
            # Exercise C/C++ ABI crossings and independently selected ownership.
            pairs = [(modes[0], modes[0]), (modes[0], modes[1]),
                     (modes[1], modes[0]), (modes[1], modes[1]),
                     (modes[2], modes[2])]
            for (provider, consumer), po, co, opt in itertools.product(
                    pairs, ('arc', 'mrc'), ('arc', 'mrc'), ('O0', 'O2')):
                label = '-'.join((provider[1], po, consumer[1], co, opt))
                for name in ('provider.h', 'provider.m', 'client.m'):
                    shutil.copyfile(fixture / name, root / name)
                common = ['--no-default-config', '-fobjc-version=3',
                          '-fobjc-runtime=gnustep-2.2', '-fblocks', '-fmodules',
                          '-fmodules-cache-path=' + str(root / 'modules'),
                          '-I' + str(include_root), '-I' + str(sdk / 'include'),
                          '-Wall', '-Wextra', '-Werror', '-Wno-missing-field-initializers',
                          '-g', '-UNDEBUG', '-' + opt]
                for part, mode, ownership in [('provider', provider, po), ('client', consumer, co)]:
                    run(label + '-' + part, [compiler, *common, '-x', mode[0],
                        '-std=' + mode[1], '-fobjc-arc' if ownership == 'arc' else '-fno-objc-arc',
                        '-c', root / (part + '.m'), '-o', root / (part + '.o')], root)
                    # The consumer cannot see or recompile the implementation.
                    (root / (part + '.m')).unlink()
                executable = root / 'client'
                run(label + '-link', [sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                    root / 'provider.o', root / 'client.o', runtime_library,
                    '-pthread', '-L' + str(sdk / 'lib'), '-lobjc',
                    '-Wl,-rpath,' + str(sdk / 'lib'),
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                    '-o', executable], root)
                run(label + '-execute', [executable], root)
                cases.append({'provider_standard': provider[1], 'consumer_standard': consumer[1],
                    'provider_ownership': po, 'consumer_ownership': co, 'optimization': opt,
                    'provider_source_removed': True,
                    'executable_sha256': packages.digest(executable)})
            if not development:
                # Exercise the installed product launcher with its own SDK
                # discovery and link defaults, without explicit include paths.
                for name in ('provider.h', 'provider.m', 'client.m'):
                    shutil.copyfile(fixture / name, root / name)
                launcher = sdk / 'bin/objc3c'
                run('launcher-provider', [launcher, root / 'provider.m', '-fmodules',
                    '-fmodules-cache-path=' + str(root / 'launcher-modules'),
                    '-std=c17', '-O2', '-c', '-o', root / 'provider.o'], root)
                (root / 'provider.m').unlink()
                run('launcher-consumer', [launcher, root / 'client.m', '-fmodules',
                    '-fmodules-cache-path=' + str(root / 'launcher-modules'),
                    '-std=c17', '-O2', root / 'provider.o', runtime_library,
                    '-lstdc++', '-pthread', '-o', root / 'launcher-client'], root)
                run('launcher-execute', [root / 'launcher-client'], root)
            invalid = root / 'invalid.m'
            invalid.write_text('@import objc3.core;\n')
            run('major-version-mismatch', [compiler, *common, '-x', 'objective-c',
                '-std=c17', '-DOBJC3_STDLIB_REQUIRED_MAJOR=99', '-fsyntax-only', invalid],
                root, failure='incompatible Objective-C 3.0 standard-library major version')
            run('wrong-language-mode', [compiler, '--no-default-config', '-x', 'objective-c',
                '-fmodules', '-fobjc-runtime=gnustep-2.2', '-fblocks',
                '-I' + str(include_root), '-I' + str(sdk / 'include'),
                '-fmodules-cache-path=' + str(root / 'wrong-mode-modules'),
                '-fsyntax-only', invalid], root,
                failure='requires Objective-C 3.0 language mode')
        if inputs() != initial_inputs:
            raise ValueError('standard-library inputs changed during qualification')
        report = {'schema': 'objc3c.stdlib-qualification.v1', 'status': 'PASS',
            'sdk_seal': manifest['seal_sha256'],
            'runtime_binding': 'development-overlay' if development else 'sealed-sdk',
            **initial_inputs,
            'executions': len(cases), 'cases': cases, 'commands': commands}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.stdlib-qualification.v1', 'status': 'FAIL',
            'sdk_seal': manifest['seal_sha256'], 'cases': cases,
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
    print(json.dumps({'status': report['status'], 'executions': report['executions'],
                      'commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
