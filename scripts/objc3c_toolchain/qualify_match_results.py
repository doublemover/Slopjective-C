"""Exercise native match result construction, ownership and PCH bodies."""
from __future__ import annotations

import argparse
import itertools
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


def qualify(sdk: Path, output: Path, *, fixture: Path | None = None,
            compiler: Path | None = None, frontend: Path | None = None,
            frontend_sdk_root: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    development = any(value is not None for value in
                      (fixture, compiler, frontend, frontend_sdk_root))
    fixture = fixture or sdk / 'share/objc3c/examples/match-results'
    compiler = compiler or sdk / 'bin/clang'
    frontend = frontend or sdk / 'bin/objc3c'
    frontend_sdk_root = frontend_sdk_root or sdk
    env = environment(sdk)
    cases, interfaces, commands = [], [], []

    def run(name, argv, cwd):
        log = output / (name + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, argv)), cwd=cwd, env=env,
                                    stdout=stream, stderr=subprocess.STDOUT, timeout=120)
        commands.append({'name': name, 'exit_code': result.returncode,
                         'log_sha256': packages.digest(log)})
        if result.returncode:
            raise ValueError('match result qualification failed: ' + name)
        return log

    try:
        source_hashes = {name: packages.digest(fixture / name) for name in
                         ('runtime.m', 'provider.m', 'consumer.m')}
        frontend_hash = packages.digest(frontend.resolve())
        compiler_hash = packages.digest(compiler.resolve())
        with tempfile.TemporaryDirectory(prefix="objc3 match 'results ") as temporary:
            root = Path(temporary)
            source = root / 'runtime.m'
            shutil.copyfile(fixture / 'runtime.m', source)
            consumer = root / 'consumer.m'
            consumer.write_text('#undef OBJC3_MATCH_PCH\n'
                                'int main(void) { return runtime_contract(); }\n')
            modes = [('objective-c', 'c17'), ('objective-c++', 'c++20'),
                     ('objective-c++', 'c++23')]
            for (language, standard), ownership, opt in itertools.product(
                    modes, ('arc', 'mrc'), ('O0', 'O2')):
                name = '-'.join((standard, ownership, opt))
                common = ['--no-default-config', '-fobjc-version=3',
                          '-fobjc-runtime=gnustep-2.2', '-fblocks', '-fexceptions',
                          '-fobjc-arc-exceptions', '-std=' + standard,
                          '-fobjc-arc' if ownership == 'arc' else '-fno-objc-arc',
                          '-I' + str(sdk / 'include'), '-' + opt, '-g', '-UNDEBUG',
                          '-Wall', '-Wextra', '-Werror']
                run(name + '-compile', [compiler, *common, '-x', language,
                    '-c', source, '-o', root / 'runtime.o'], root)
                binaries, debugger_values = {}, {}
                for kind in ('ordinary', 'pch'):
                    if kind == 'pch':
                        run(name + '-pch-generate', [compiler, *common,
                            '-x', language + '-header', '-DOBJC3_MATCH_PCH=1',
                            source, '-o', root / 'runtime.pch'], root)
                        run(name + '-pch-consume', [compiler, *common,
                            '-x', language, '-include-pch', root / 'runtime.pch',
                            '-c', consumer, '-o', root / 'runtime.o'], root)
                    binary = root / 'application'
                    run(name + '-' + kind + '-link', [sdk / 'bin/clang++',
                        '--no-default-config', '-fuse-ld=lld', root / 'runtime.o',
                        '-L' + str(sdk / 'lib'), '-lobjc', '-Wl,-rpath,' + str(sdk / 'lib'),
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                        '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                        '-o', binary], root)
                    run(name + '-' + kind + '-execute', [binary], root)
                    binaries[kind] = packages.digest(binary)
                    if opt == 'O0' and language == 'objective-c++':
                        debugger = [sdk / 'bin/lldb', '--batch']
                        for query in ('settings set target.disable-aslr false',
                                      'breakpoint set -n match_value_checkpoint', 'run',
                                      'expression -- observed', 'frame select 1',
                                      'expression -- (int)(copied.value == 10 && Lifetime::live == 3)',
                                      'breakpoint disable 1', 'continue'):
                            debugger += ['-o', query]
                        log = run(name + '-' + kind + '-debugger', [*debugger, binary], root)
                        text = log.read_text()
                        values = list(map(int, re.findall(r'\(int\) \$\d+ = (-?\d+)', text)))
                        if values != [10, 1] or 'exited with status = 0' not in text:
                            raise ValueError('match result debugger values missing: ' + name + '-' + kind)
                        debugger_values[kind] = values
                cases.append({'language': language, 'standard': standard,
                              'ownership': ownership, 'optimization': opt,
                              'executable_hashes': binaries, 'debugger_values': debugger_values})
            for (language, standard), opt in itertools.product(modes, ('O0', 'O2')):
                name = standard + '-interface-' + opt
                provider, consumer = root / 'provider.m', root / 'module-consumer.m'
                shutil.copyfile(fixture / 'provider.m', provider)
                shutil.copyfile(fixture / 'consumer.m', consumer)
                module = root / 'MatchResults.objc3i'
                frontend_common = ['--objc3', '--arc', '--sdk-root', frontend_sdk_root]
                if language == 'objective-c++':
                    frontend_common.append('--objcxx')
                flags = ['--', '--no-default-config', '-std=' + standard,
                         '-fblocks', '-I' + str(sdk / 'include'), '-' + opt, '-g']
                run(name + '-provider', [frontend, provider, *frontend_common,
                    '--emit-object', root / 'provider.o', '--emit-interface', module,
                    '--module', 'fixture.MatchResults', '--package', 'match-results-v1',
                    '--export-declaration', 'MatchOptional',
                    '--export-declaration', 'MatchResult',
                    '--export-declaration', 'match_optional',
                    '--export-declaration', 'match_result', *flags], root)
                provider.unlink()
                identity = json.loads(module.with_suffix('.objc3i.json').read_text())['module']
                run(name + '-consumer', [frontend, consumer, *frontend_common,
                    '--source-imports', '--module-search-path', root, '--import-module',
                    'fixture.MatchResults', identity['semantic_fingerprint'], 'match-results-v1',
                    '--emit-object', root / 'consumer.o', *flags], root)
                binary = root / 'module-consumer'
                run(name + '-link', [sdk / 'bin/clang++', '--no-default-config',
                    '-fuse-ld=lld', root / 'provider.o', root / 'consumer.o',
                    '-L' + str(sdk / 'lib'), '-lobjc', '-Wl,-rpath,' + str(sdk / 'lib'),
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_addr=0xffffffffffffffff',
                    '-Wl,-z,dead-reloc-in-nonalloc=.debug_info=0xffffffffffffffff',
                    '-o', binary], root)
                run(name + '-execute', [binary], root)
                interfaces.append({'language': language, 'standard': standard,
                    'optimization': opt, 'provider_source_removed': True,
                    'semantic_fingerprint': identity['semantic_fingerprint'],
                    'executable_sha256': packages.digest(binary)})
        if ({name: packages.digest(fixture / name) for name in source_hashes} != source_hashes or
                packages.digest(compiler.resolve()) != compiler_hash or
                packages.digest(frontend.resolve()) != frontend_hash):
            raise ValueError('match result inputs changed during qualification')
        report = {'schema': 'objc3c.match-result-qualification.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'],
                  'compiler_binding': 'development-overlay' if development else 'sealed-sdk',
                  'compiler_sha256': compiler_hash, 'frontend_sha256': frontend_hash,
                  'source_hashes': source_hashes,
                  'configurations': len(cases), 'executions': 2 * len(cases) + len(interfaces),
                  'source_free_interface_cases': interfaces,
                  'debugger_cases': sum(len(case['debugger_values']) for case in cases),
                  'cases': cases, 'commands': commands}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.match-result-qualification.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'], 'cases': cases,
            'source_free_interface_cases': interfaces,
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
    print(json.dumps({'status': report['status'], 'configurations': report['configurations'],
                      'executions': report['executions'], 'commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
