"""Qualify managed Objective-C and Block call ownership using only an installed Linux SDK."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
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
    names = ('carrier-call.h', 'carrier-call-provider.m', 'carrier-runtime-call.h',
             'carrier-runtime-provider.m', 'carrier-runtime-main.m',
             'carrier-runtime-module.m', 'carrier-runtime-import.m')
    for name in names:
        shutil.copyfile(Path(__file__).parent / name, sources / name)
    seal = json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256']
    report = {'schema': 'objc3c.installed-runtime-carrier-calls.v1', 'status': 'running',
              'sdk_seal': seal, 'commands': [], 'executions': [],
              'source_hashes': {name: hashlib.sha256((sources / name).read_bytes()).hexdigest()
                                for name in names}}
    path = output / 'qualification.json'
    env = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib')}
    compiler = str(sdk / 'bin/objc3c')
    def save():
        path.write_text(json.dumps(report, indent=2) + '\n')
    def run(name, args):
        started = time.monotonic()
        with (output / (name + '.log')).open('w') as log:
            result = subprocess.run(list(map(str, args)), cwd=sources, env=env,
                                    stdout=log, stderr=subprocess.STDOUT, timeout=180)
        report['commands'].append({'name': name, 'exit_code': result.returncode, 'expected_exit_code': 0,
                                   'elapsed_seconds': round(time.monotonic() - started, 6)})
        save()
        if result.returncode:
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
                         '-fobjc-arc-exceptions', '-g', '-' + optimization.split('-')[0]]
                if optimization.endswith('thinlto'):
                    flags += ['-flto=thin']
                base, provider, consumer = [output / (key + '-' + part + '.o')
                                            for part in ('base', 'provider', 'consumer')]
                run(key + '-base', [compiler, *flags, '-c', 'carrier-call-provider.m', '-o', base])
                run(key + '-provider', [compiler, *flags, '-c', 'carrier-runtime-provider.m', '-o', provider])
                run(key + '-consumer', [compiler, *flags, '-c', 'carrier-runtime-main.m', '-o', consumer])
                executable = output / key
                link_flags = ['-O2', '-flto=thin'] if optimization.endswith('thinlto') else []
                run(key + '-link', [compiler, *link_flags, base, provider, consumer, '-o', executable])
                execute(key, executable)
                if language == 'objective-c' and optimization in ('O0', 'O2'):
                    module_directory = output / (key + '-module')
                    module_directory.mkdir(exist_ok=True)
                    interface = module_directory / 'RuntimeCarrierCalls.objc3i'
                    run(key + '-interface', [compiler, 'carrier-runtime-module.m', '--objc3', '--arc',
                        '--emit-interface', interface, '--module', 'RuntimeCarrierCalls', '--package', 'runtime-carrier-calls-v1',
                        '--', '-' + optimization, '-fblocks'])
                    identity = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                    imported = output / (key + '-import.o')
                    run(key + '-import', [compiler, 'carrier-runtime-import.m', '--objc3', '--arc',
                        '--source-imports', '--module-search-path', module_directory, '--import-module', 'RuntimeCarrierCalls',
                        identity['semantic_fingerprint'], 'runtime-carrier-calls-v1', '--emit-object', imported,
                        '--emit-dependency-record', output / (key + '-dependencies.json'), '--',
                        '-' + optimization, '-g', '-fblocks'])
                    application = output / (key + '-import')
                    run(key + '-import-link', [compiler, base, provider, imported, '-o', application])
                    execute(key + '-import', application)
                    report.setdefault('interfaces', []).append({'optimization': optimization,
                         'semantic_fingerprint': identity['semantic_fingerprint'],
                         'runtime_abi': identity['runtime_abi']})
        flags = ['-std=c++20', '-fexceptions', '-fobjc-arc-exceptions', '-O2', '-g']
        pch = output / 'calls.pch'
        run('pch-header', [compiler, '-x', 'objective-c++-header', *flags, '-c', 'carrier-runtime-call.h', '-o', pch])
        consumer = output / 'pch-consumer.o'
        run('pch-consumer', [compiler, '-x', 'objective-c++', *flags, '-include-pch', pch,
                             '-c', 'carrier-runtime-main.m', '-o', consumer])
        executable = output / 'pch-application'
        run('pch-link', [compiler, output / 'objective-c++-O2-base.o', output / 'objective-c++-O2-provider.o', consumer, '-o', executable])
        execute('pch-application', executable)
        assert len(report['executions']) == 9
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
    print(json.dumps({'status': result['status'], 'runtime_call_executions': len(result['executions']),
                      'commands': len(result['commands'])}))
