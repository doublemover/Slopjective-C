"""Qualify GNUstep 2.2 direct managed calls using only an installed Linux SDK."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import time


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(), output.resolve()
    if output == sdk or sdk in output.parents or output in sdk.parents:
        raise ValueError('qualification output must be disjoint from the SDK')
    output.mkdir(parents=True, exist_ok=True)
    sources = output / 'sources with spaces and quotes\''
    sources.mkdir(exist_ok=True)
    names = ('carrier-call.h', 'carrier-call-provider.m', 'carrier-throws-call.h', 'carrier-throws-provider.m', 'carrier-direct-call.h',
             'carrier-direct-provider.m', 'carrier-direct-main.m',
             'carrier-direct-module.m', 'carrier-direct-import.m')
    for name in names:
        shutil.copyfile(Path(__file__).parent / name, sources / name)
    seal = json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256']
    report = {'schema': 'objc3c.installed-direct-carrier-calls.v1', 'status': 'running',
              'sdk_seal': seal, 'commands': [], 'executions': [],
              'forced_failures': [], 'source_hashes': {name: hashlib.sha256((sources / name).read_bytes()).hexdigest()
                                for name in names}}
    path = output / 'qualification.json'
    env = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib')}
    compiler = str(sdk / 'bin/objc3c')
    def save():
        path.write_text(json.dumps(report, indent=2) + '\n')
    def run(name, args, expected=0):
        started = time.monotonic()
        with (output / (name + '.log')).open('w') as log:
            result = subprocess.run(list(map(str, args)), cwd=sources, env=env,
                                    stdout=log, stderr=subprocess.STDOUT, timeout=180)
        report['commands'].append({'name': name, 'exit_code': result.returncode,
                                   'expected_exit_code': expected, 'elapsed_seconds': round(time.monotonic() - started, 6)})
        save()
        if result.returncode != expected:
            raise ValueError(f'{name} failed; see {output / (name + ".log")}')
    def execute(name, executable):
        run(name, [executable])
        run(name + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable])
        report['executions'].append({'name': name, 'executable_sha256':
                                     hashlib.sha256(executable.read_bytes()).hexdigest()})
        save()
    try:
        run('seal-before', [sys.executable, '-B', sdk / 'share/objc3c/verify-sdk.py', sdk])
        for language, standard in (('objective-c', 'c17'), ('objective-c++', 'c++20')):
            for optimization in ('O0', 'O2', 'O2-thinlto'):
                key = language + '-' + optimization
                flags = ['-x', language, '-std=' + standard, '-fexceptions',
                         '-fobjc-arc-exceptions', '-pthread', '-Werror=ignored-attributes', '-g', '-' + optimization.split('-')[0]]
                if optimization.endswith('thinlto'):
                    flags += ['-flto=thin']
                base, errors, provider, consumer = [output / (key + '-' + part + '.o')
                                            for part in ('base', 'errors', 'provider', 'consumer')]
                run(key + '-base', [compiler, *flags, '-c', 'carrier-call-provider.m', '-o', base])
                run(key + '-errors', [compiler, *flags, '-c', 'carrier-throws-provider.m', '-o', errors])
                run(key + '-provider', [compiler, *flags, '-c', 'carrier-direct-provider.m', '-o', provider])
                run(key + '-consumer', [compiler, *flags, '-c', 'carrier-direct-main.m', '-o', consumer])
                executable = output / key
                link_flags = ['-pthread'] + (['-O2', '-flto=thin'] if optimization.endswith('thinlto') else [])
                run(key + '-link', [compiler, *link_flags, base, errors, provider, consumer, '-o', executable])
                execute(key, executable)
                for mode in ('force-class', 'force-instance'):
                    run(key + '-' + mode, [executable, mode], expected=-signal.SIGILL)
                    report['forced_failures'].append({'configuration': key, 'mode': mode,
                                                       'signal': 'SIGILL'})
                if language == 'objective-c' and optimization in ('O0', 'O2'):
                    module_directory = output / (key + '-module')
                    module_directory.mkdir(exist_ok=True)
                    interface = module_directory / 'DirectCarrierCalls.objc3i'
                    run(key + '-interface', [compiler, 'carrier-direct-module.m', '--objc3', '--arc',
                        '--emit-interface', interface, '--module', 'DirectCarrierCalls', '--package', 'direct-carrier-calls-v1',
                        '--', '-' + optimization, '-fblocks'])
                    identity = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                    imported = output / (key + '-import.o')
                    run(key + '-import', [compiler, 'carrier-direct-import.m', '--objc3', '--arc',
                        '--source-imports', '--module-search-path', module_directory, '--import-module', 'DirectCarrierCalls',
                        identity['semantic_fingerprint'], 'direct-carrier-calls-v1', '--emit-object', imported,
                        '--emit-dependency-record', output / (key + '-dependencies.json'), '--',
                        '-' + optimization, '-g', '-fblocks'])
                    application = output / (key + '-import')
                    run(key + '-import-link', [compiler, base, errors, provider, imported, '-o', application])
                    execute(key + '-import', application)
                    report.setdefault('interfaces', []).append({'optimization': optimization,
                         'semantic_fingerprint': identity['semantic_fingerprint'],
                         'runtime_abi': identity['runtime_abi']})
        flags = ['-std=c++20', '-fexceptions', '-fobjc-arc-exceptions', '-O2', '-g', '-pthread', '-Werror=ignored-attributes']
        pch = output / 'calls.pch'
        run('pch-header', [compiler, '-x', 'objective-c++-header', *flags, '-c', 'carrier-direct-call.h', '-o', pch])
        consumer = output / 'pch-consumer.o'
        run('pch-consumer', [compiler, '-x', 'objective-c++', *flags, '-include-pch', pch,
                             '-c', 'carrier-direct-main.m', '-o', consumer])
        executable = output / 'pch-application'
        run('pch-link', [compiler, '-pthread', output / 'objective-c++-O2-base.o', output / 'objective-c++-O2-errors.o', output / 'objective-c++-O2-provider.o', consumer, '-o', executable])
        execute('pch-application', executable)
        report['runtime_rejections'] = []
        for runtime in ('gnustep-2.0', 'gnustep-2.1', 'gcc'):
            for mode in ('driver', 'response', 'interface'):
                name = 'reject-' + runtime + '-' + mode
                target = output / (name + '.o')
                if mode == 'interface':
                    args = ['carrier-direct-module.m', '--objc3', '--arc', '--runtime', runtime,
                            '--emit-object', target, '--', '-fblocks']
                    expected = 1
                else:
                    args = ['-x', 'objective-c', '-fobjc-runtime=' + runtime,
                            '-c', 'carrier-direct-main.m', '-o', target]
                    expected = 2
                    if mode == 'response':
                        response = sources / (name + '.rsp')
                        response.write_text(' '.join('"' + str(arg).replace('\\', '\\\\').replace('"', '\\"') + '"' for arg in args) + '\n')
                        args = ['@' + str(response)]
                run(name, [compiler, *args], expected=expected)
                assert 'requires GNUstep 2.2' in (output / (name + '.log')).read_text()
                assert not target.exists()
                report['runtime_rejections'].append({'runtime': runtime, 'mode': mode})
        assert len(report['executions']) == 9
        assert len(report['forced_failures']) == 12
        assert report['interfaces'][0]['semantic_fingerprint'] == report['interfaces'][1]['semantic_fingerprint']
        assert all(row['runtime_abi'] == 'gnustep-2.2;arc' for row in report['interfaces'])
        run('seal-after', [sys.executable, '-B', sdk / 'share/objc3c/verify-sdk.py', sdk])
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        save()
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.output_dir)
    print(json.dumps({'status': result['status'], 'direct_call_executions': len(result['executions']),
                      'forced_failures': len(result['forced_failures']), 'runtime_rejections': len(result['runtime_rejections']), 'commands': len(result['commands'])}))
