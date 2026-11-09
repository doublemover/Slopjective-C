"""Qualify bounded C++ semantic interfaces and exact dialect/ownership locks."""
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
    sources = output / "C++ sources with spaces and 'quotes'"
    sources.mkdir(exist_ok=True)
    names = ('carrier-record-call.h', 'carrier-record-native.m', 'cpp-data-module.mm',
             'cpp-data-provider.mm', 'cpp-data-import.mm', 'cpp-data-rejected.mm',
             'cpp-data-published.mm',
             'cpp-data-import-rejected.mm', 'cpp-plain-module.cpp',
             'cpp-plain-provider.cpp', 'cpp-plain-import.cpp')
    for name in names:
        shutil.copyfile(Path(__file__).parent / name, sources / name)
    report = {'schema': 'objc3c.installed-cxx-interfaces.v1', 'status': 'running',
              'sdk_seal': json.loads((sdk / 'objc3-sdk.json').read_text())['seal_sha256'],
              'commands': [], 'executions': [], 'rejections': [], 'admissions': [], 'interfaces': [],
              'source_hashes': {name: hashlib.sha256((sources / name).read_bytes()).hexdigest()
                                for name in names}}
    path = output / 'qualification.json'
    compiler = sdk / 'bin/objc3c'
    env = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib')}

    def save():
        path.write_text(json.dumps(report, indent=2) + '\n')

    def run(name, args, expected=0, diagnostic=None):
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
        if expected:
            text = (output / (name + '.log')).read_text()
            if not any(word in text for word in ('error:', 'O3IFC')) or any(
                    word in text for word in ('PLEASE submit a bug report', 'Assertion failed')):
                raise ValueError(name + ' failed without a source diagnostic')
            if diagnostic and diagnostic not in text:
                raise ValueError(name + ' failed without its required diagnostic '+diagnostic)
            report['rejections'].append({'name': name, 'diagnostic': diagnostic})

    def import_command(standard, directory, identity, source, obj, *, arc=True):
        return [compiler, source, '--objc3', '--objcxx', *(['--arc'] if arc else []),
                '--source-imports', '--module-search-path', directory,
                '--import-module', 'CxxData', identity['semantic_fingerprint'], 'cpp-data-v1',
                '--emit-object', obj, '--emit-dependency-record', obj.with_suffix('.json'),
                '--', '-std=' + standard, '-fblocks', '-fexceptions', '-fobjc-arc-exceptions', '-g']

    try:
        run('seal-before', [sys.executable, '-B', sdk / 'share/objc3c/verify-sdk.py', sdk])
        for standard in ('c++20', 'c++23'):
            for optimization in ('O0', 'O2', 'O2-thinlto'):
                key = standard + '-' + optimization
                flags = ['-' + optimization.split('-')[0], '-g', '-fexceptions', '-fobjc-arc-exceptions']
                if optimization.endswith('thinlto'):
                    flags.append('-flto=thin')
                native, provider, consumer = [output / (key + '-' + part + '.o')
                                               for part in ('native', 'provider', 'consumer')]
                run(key + '-native', [compiler, '-x', 'objective-c', '-std=c17', *flags,
                                      '-c', 'carrier-record-native.m', '-o', native])
                run(key + '-provider', [compiler, '-std=' + standard, *flags,
                                        '-c', 'cpp-data-provider.mm', '-o', provider])
                directory = output / (key + '-module')
                directory.mkdir(exist_ok=True)
                interface = directory / 'CxxData.objc3i'
                run(key + '-interface', [compiler, 'cpp-data-module.mm', '--objc3', '--objcxx', '--arc',
                    '--emit-interface', interface, '--module', 'CxxData', '--package', 'cpp-data-v1',
                    '--', '-std=' + standard, '-fblocks', *flags])
                sidecar = json.loads(interface.with_suffix('.objc3i.json').read_text())
                identity = sidecar['module']
                if not ('objc3.cxx-data-interfaces.v1' in sidecar['required_semantics']):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:87')
                if not (';language=objective-c++;' in identity['language_profile']):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:88')
                if not (';standard=' + standard + ';' in identity['language_profile']):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:89')
                run(key + '-consumer', [*import_command(standard, directory, identity,
                    'cpp-data-import.mm', consumer), *flags])
                executable = output / key
                run(key + '-link', [compiler, *flags, native, provider, consumer, '-o', executable])
                run(key + '-execution', [executable])
                run(key + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable])
                report['executions'].append({'name': key, 'executable_sha256':
                                             hashlib.sha256(executable.read_bytes()).hexdigest()})
                report['interfaces'].append({'standard': standard, 'optimization': optimization,
                                             'language': 'objective-c++', 'identity': identity})
        for standard in ('c++20', 'c++23'):
            for optimization in ('O0', 'O2', 'O2-thinlto'):
                key = 'plain-' + standard + '-' + optimization
                flags = ['-' + optimization.split('-')[0], '-g']
                if optimization.endswith('thinlto'):
                    flags.append('-flto=thin')
                provider, consumer = [output / (key + '-' + part + '.o')
                                       for part in ('provider', 'consumer')]
                run(key + '-provider', [compiler, '-std=' + standard, *flags,
                                        '-c', 'cpp-plain-provider.cpp', '-o', provider])
                directory = output / (key + '-module')
                directory.mkdir(exist_ok=True)
                interface = directory / 'CxxPlain.objc3i'
                run(key + '-interface', [compiler, 'cpp-plain-module.cpp', '--emit-interface',
                    interface, '--module', 'CxxPlain', '--package', 'cpp-data-v1', '--',
                    '-std=' + standard, *flags])
                sidecar = json.loads(interface.with_suffix('.objc3i.json').read_text())
                identity = sidecar['module']
                if not ('objc3.cxx-data-interfaces.v1' in sidecar['required_semantics']):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:118')
                if not (';language=c++;' in identity['language_profile']):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:119')
                run(key + '-consumer', [compiler, 'cpp-plain-import.cpp',
                    '--module-search-path', directory, '--import-module', 'CxxPlain',
                    identity['semantic_fingerprint'], 'cpp-data-v1', '--emit-object', consumer,
                    '--emit-dependency-record', consumer.with_suffix('.json'), '--',
                    '-std=' + standard, *flags])
                executable = output / key
                run(key + '-link', [compiler, *flags, provider, consumer, '-o', executable])
                run(key + '-execution', [executable])
                run(key + '-dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable])
                report['executions'].append({'name': key, 'executable_sha256':
                                             hashlib.sha256(executable.read_bytes()).hexdigest()})
                report['interfaces'].append({'standard': standard, 'optimization': optimization,
                                             'language': 'c++', 'identity': identity})
        # Former rejection cases now belong to the supported publication surface.
        for case in range(1, 17):
            directory = output / ('published-' + str(case) + '-module')
            directory.mkdir()
            interface, provider = directory / 'Published.objc3i', directory / 'provider.o'
            run('published-interface-' + str(case), [compiler, 'cpp-data-published.mm', '--objc3',
                '--objcxx', '--arc', '--emit-interface', interface, '--module', 'Published',
                '--package', 'cpp-data-v1', *(['--emit-object', provider] if case in (15, 16) else []),
                '--', '-std=c++20', '-DCASE=' + str(case)])
            sidecar = json.loads(interface.with_suffix('.objc3i.json').read_text())
            if not interface.is_file() or not sidecar['module']['semantic_fingerprint']:
                raise ValueError('supported publication did not emit a complete interface')
            if case in (15, 16):
                client = sources / ('published-client-'+str(case)+'.mm')
                call = ('Methods<int> value; return value.get<int>(41) != 42;'
                        if case == 15 else 'auto value = @Published.api(); return value(41) != 42;')
                client.write_text('@import Published;\nint main() { '+call+' }\n')
                original = sources / 'cpp-data-published.mm'
                held = output / ('held-provider-'+str(case)+'.mm')
                original.rename(held)
                obj, binary = output / ('published-client-'+str(case)+'.o'), output / ('published-client-'+str(case))
                run('published-consumer-'+str(case), [compiler, client, '--objc3', '--objcxx', '--arc',
                    '--source-imports', '--module-search-path', directory, '--import-module', 'Published',
                    sidecar['module']['semantic_fingerprint'], 'cpp-data-v1', '--emit-object', obj,
                    '--', '-std=c++20'])
                run('published-link-'+str(case), [sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
                    provider, obj, '-L'+str(sdk / 'lib'), '-lobjc', '-Wl,-rpath,'+str(sdk / 'lib'), '-o', binary])
                run('published-execute-'+str(case), [binary])
                report['executions'].append({'case': case, 'source_free': True,
                    'executable_sha256': hashlib.sha256(binary.read_bytes()).hexdigest()})
                held.rename(original)
        for case in (1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 14):
            admitted = case in (2, 3, 4, 5, 8, 10, 11, 14)
            interface = output / ('rejected-' + str(case) + '.objc3i')
            run('rejected-interface-' + str(case), [compiler, 'cpp-data-rejected.mm', '--objc3',
                '--objcxx', '--arc', '--emit-interface', interface, '--module', 'Rejected',
                '--package', 'cpp-data-v1', '--', '-std=c++20', '-DCASE=' + str(case)], expected=0 if admitted else 1,
                diagnostic=None if admitted else ('O3IFC112' if case == 7 else 'O3IFC101'))
            if admitted:
                metadata=interface.with_suffix('.objc3i.json')
                if not interface.is_file() or not metadata.is_file():
                    raise ValueError('admitted interface did not publish both artifacts')
                document=json.loads(metadata.read_text())
                if document['module']['canonical_name']!='Rejected' or not document['declarations']:
                    raise ValueError('admitted interface lost its module or declaration contract')
                report['admissions'].append({'case':case,'interface_sha256':hashlib.sha256(interface.read_bytes()).hexdigest(),
                    'metadata_sha256':hashlib.sha256(metadata.read_bytes()).hexdigest()})
                continue
            if interface.exists() or interface.with_suffix('.objc3i.json').exists():
                raise ValueError('rejected interface left published output')
        directory = output / 'c++20-O0-module'
        identity = next(row['identity'] for row in report['interfaces']
                        if row['standard'] == 'c++20' and row['optimization'] == 'O0')
        for name, standard, arc in (('standard', 'c++23', True), ('ownership', 'c++20', False)):
            obj = output / ('rejected-' + name + '.o')
            run('rejected-' + name, import_command(standard, directory, identity,
                'cpp-data-import.mm', obj, arc=arc), expected=1)
            if obj.exists() or obj.with_suffix('.json').exists():
                raise ValueError('rejected import left published object or dependency output')
        for case in (1, 2):
            obj = output / ('rejected-visibility-' + str(case) + '.o')
            run('rejected-visibility-' + str(case), [*import_command('c++20', directory, identity,
                'cpp-data-import-rejected.mm', obj), '-DCASE=' + str(case)], expected=1)
        if len(report['executions']) != 14 or len(report['rejections']) != 16:
            raise ValueError('C++ interface execution/rejection inventory is incomplete')
        if len(report['commands']) != 117:
            raise ValueError('C++ interface command inventory is incomplete')
        for language in ('c++', 'objective-c++'):
            for standard in ('c++20', 'c++23'):
                if not (len({row['identity']['semantic_fingerprint'] for row in report['interfaces'] if row['standard'] == standard and row['language'] == language}) == 1):
                    raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:190')
        if not (len({row['identity']['semantic_fingerprint'] for row in report['interfaces']}) == 4):
            raise ValueError('installed qualification contract failed: qualify-cpp-interfaces.py:192')
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
    print(json.dumps({'status': result['status'], 'cxx_interface_executions': len(result['executions']),
                      'rejections': len(result['rejections']), 'commands': len(result['commands'])}))
