"""Qualify Objective-C storage, runtime record calls, and shared Block captures."""
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
    sources = output / "heap sources with spaces and 'quotes'"
    sources.mkdir(exist_ok=True)
    names = ('carrier-record-call.h', 'carrier-record-native.m',
             'carrier-heap-call.h', 'carrier-heap-provider.m', 'carrier-heap-main.m',
             'carrier-heap-module.m', 'carrier-heap-import.m', 'carrier-heap-rejected.m')
    for name in names:
        shutil.copyfile(Path(__file__).parent / name, sources / name)
    report = {'schema': 'objc3c.installed-heap-carriers.v1', 'status': 'running',
              'sdk_seal': json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
              'commands': [], 'executions': [], 'rejections': [], 'interfaces': [],
              'source_hashes': {name: hashlib.sha256((sources / name).read_bytes()).hexdigest()
                                for name in names}}
    path = output / 'qualification.json'
    compiler = sdk / 'bin/objc3c'
    env = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib')}

    def save():
        path.write_text(json.dumps(report, indent=2) + '\n')

    def run(name, args, expected=0):
        started = time.monotonic()
        with (output / (name + '.log')).open('w') as log:
            result = subprocess.run(list(map(str, args)), cwd=sources, env=env,
                                    stdout=log, stderr=subprocess.STDOUT, timeout=180)
        report['commands'].append({'name': name, 'exit_code': result.returncode,
                                   'expected_exit_code': expected,
                                   'elapsed_seconds': round(time.monotonic() - started, 6)})
        save()
        if result.returncode != expected:
            raise ValueError(f'{name} failed; see {output / (name + ".log")}')

    def execute(name, executable):
        run(name, [executable])
        run(name + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable])
        report['executions'].append({'name': name, 'executable_sha256':
                                     hashlib.sha256(executable.read_bytes()).hexdigest()})
        save()

    def flags(language, optimization):
        return ['-x', language, '-std=' + ('c++20' if language.endswith('++') else 'c17'),
                '-fexceptions', '-fobjc-arc-exceptions', '-Werror=return-type-c-linkage',
                '-Werror=ignored-attributes', '-g', '-' + optimization.split('-')[0],
                *(['-flto=thin'] if optimization.endswith('thinlto') else [])]

    try:
        run('seal-before', [sys.executable, '-B', sdk / 'share/objc3c/verify-sdk.py', sdk])
        for provider_language, consumer_language in (
                ('objective-c', 'objective-c'), ('objective-c++', 'objective-c++'),
                ('objective-c', 'objective-c++'), ('objective-c++', 'objective-c')):
            for optimization in ('O0', 'O2', 'O2-thinlto'):
                key = provider_language + '-to-' + consumer_language + '-' + optimization
                native, provider, consumer = [output / (key + '-' + part + '.o')
                                               for part in ('native', 'provider', 'consumer')]
                run(key + '-native', [compiler, *flags('objective-c', optimization),
                                      '-c', 'carrier-record-native.m', '-o', native])
                run(key + '-provider', [compiler, *flags(provider_language, optimization),
                                        '-c', 'carrier-heap-provider.m', '-o', provider])
                run(key + '-consumer', [compiler, *flags(consumer_language, optimization),
                                        '-c', 'carrier-heap-main.m', '-o', consumer])
                executable = output / key
                link_flags = ['-O2', '-flto=thin'] if optimization.endswith('thinlto') else []
                run(key + '-link', [compiler, *link_flags, native, provider, consumer, '-o', executable])
                execute(key, executable)
                if provider_language == consumer_language == 'objective-c' and optimization in ('O0', 'O2'):
                    directory = output / (key + '-module')
                    directory.mkdir(exist_ok=True)
                    module_name = 'HeapStorageC'
                    interface = directory / (module_name + '.objc3i')
                    run(key + '-interface', [compiler, 'carrier-heap-module.m', '--objc3', '--arc',
                        '--emit-interface', interface, '--module', module_name, '--package', 'heap-carriers-v1',
                        '--', '-std=c17', '-' + optimization, '-fblocks'])
                    identity = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                    imported = output / (key + '-import.o')
                    run(key + '-import-object', [compiler, 'carrier-heap-import.m', '--objc3', '--arc',
                        '--source-imports', '--module-search-path', directory, '--import-module', module_name,
                        identity['semantic_fingerprint'], 'heap-carriers-v1', '--emit-object', imported,
                        '--emit-dependency-record', output / (key + '-dependencies.json'), '--',
                        '-std=c17', '-' + optimization, '-g', '-fblocks'])
                    application = output / (key + '-import')
                    run(key + '-import-link', [compiler, native, provider, imported, '-o', application])
                    execute(key + '-import', application)
                    report['interfaces'].append({'language': provider_language, 'optimization': optimization,
                         'semantic_fingerprint': identity['semantic_fingerprint'],
                         'runtime_abi': identity['runtime_abi']})
        for language in ('objective-c', 'objective-c++'):
            for case in (1, 2, 3, 4, 5, 6):
                key = language + '-rejected-' + str(case)
                run(key, [compiler, *flags(language, 'O0'), '-DCASE=' + str(case),
                          '-c', 'carrier-heap-rejected.m', '-o', output / 'rejected.o'], expected=1)
                text = (output / (key + '.log')).read_text()
                if 'error:' not in text or any(word in text for word in ('PLEASE submit a bug report', 'Assertion failed')):
                    raise ValueError(key + ' failed without a source diagnostic')
                report['rejections'].append({'language': language, 'case': case})
        pch = output / 'records.pch'
        pch_flags = flags('objective-c++', 'O2')[2:]
        run('pch-header', [compiler, '-x', 'objective-c++-header', *pch_flags,
                           '-c', 'carrier-heap-call.h', '-o', pch])
        consumer = output / 'pch-consumer.o'
        run('pch-consumer', [compiler, *flags('objective-c++', 'O2'), '-include-pch', pch,
                             '-c', 'carrier-heap-main.m', '-o', consumer])
        key = 'objective-c++-to-objective-c++-O2'
        executable = output / 'pch-application'
        run('pch-link', [compiler, output / (key + '-native.o'), output / (key + '-provider.o'),
                         consumer, '-o', executable])
        execute('pch-application', executable)
        assert len(report['executions']) == 15
        assert len(report['rejections']) == 12
        assert len(report['interfaces']) == 2
        assert len({row['semantic_fingerprint'] for row in report['interfaces']}) == 1
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
    print(json.dumps({'status': result['status'], 'heap_carrier_executions': len(result['executions']),
                      'rejections': len(result['rejections']), 'commands': len(result['commands'])}))
