"""Check source-free factory interfaces with development compiler/runtime inputs.

The installed SDK supplies platform headers only. This does not produce or
qualify an SDK archive. Immutable package checks use a development-input lock.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from . import binary_packages as packages
from .qualify_native_async_packages import check_cpp_metadata, check_factory_metadata

ROOT = Path(__file__).resolve().parents[2]
MODULE = 'fixture.NativeAsync'


def qualify(sdk: Path, compiler: Path, foundation: Path, runtime: Path,
            objc_runtime: Path, output: Path, *, full=False, eager=False,
            sanitizer: str | None = None, resource_root: Path | None = None) -> dict:
    if sanitizer not in (None, 'address,undefined', 'thread'):
        raise ValueError('unsupported sanitizer')
    if sanitizer and resource_root is None:
        raise ValueError('sanitizer checks require an explicit compiler resource root')
    instrumentation = []
    if resource_root is not None:
        resource_root = resource_root.resolve(strict=True)
        instrumentation += ['-resource-dir', str(resource_root)]
    if sanitizer:
        instrumentation += ['-fsanitize=' + sanitizer, '-fno-omit-frame-pointer']
    sdk, compiler, foundation, runtime, objc_runtime = (
        path.resolve(strict=True) for path in
        (sdk, compiler, foundation, runtime, objc_runtime))
    output = output.resolve()
    fixture = ROOT / 'tests/clang_foundation/native_async_packages'
    headers = ROOT / 'native/objc3c/src/runtime/public'
    for protected in (sdk, compiler, foundation, runtime, objc_runtime, fixture,
                      headers, ROOT / 'stdlib/include'):
        packages.disjoint(output, protected)
    output.mkdir(parents=True, exist_ok=True)
    inputs = {str(path): packages.digest(path) for path in
              (compiler, foundation / 'bin/clang', runtime, objc_runtime)}
    if sanitizer:
        names = ('tsan',) if sanitizer == 'thread' else ('asan', 'ubsan')
        sanitizer_libraries = [path for path in resource_root.rglob('libclang_rt.*')
                               if path.is_file() and any(name in path.name for name in names)]
        if not sanitizer_libraries:
            raise ValueError('compiler resource root has no requested sanitizer libraries')
        inputs.update({str(path): packages.digest(path) for path in sanitizer_libraries})
    sources = [Path(__file__), *sorted(fixture.rglob('*')),
               *sorted(headers.rglob('*')), *sorted((ROOT / 'stdlib/include').rglob('*'))]
    source_inputs = {str(path): packages.digest(path) for path in sources if path.is_file()}
    development_lock = packages.content_hash(inputs)
    report = {'schema': 'objc3c.linux-development-factory-batch.v1',
              'status': 'running', 'development_input_lock': development_lock,
              'inputs': inputs, 'source_inputs': source_inputs,
              'full_fixture': full, 'eager': eager,
              'sanitizer': sanitizer,
              'resource_root': str(resource_root) if resource_root else None,
              'cases': [], 'commands': []}
    environment = {**os.environ, 'LD_LIBRARY_PATH': str(objc_runtime.parent)}
    if sanitizer:
        environment.setdefault('ASAN_OPTIONS', 'detect_leaks=1:halt_on_error=1')
        environment.setdefault('UBSAN_OPTIONS', 'halt_on_error=1:print_stacktrace=1')
        environment.setdefault('TSAN_OPTIONS', 'halt_on_error=1')

    def run(name, command, *, expected=0):
        log = output / (name + '.log')
        with log.open('w') as stream:
            result = subprocess.run(list(map(str, command)), env=environment,
                                    stdout=stream, stderr=subprocess.STDOUT,
                                    timeout=180)
        report['commands'].append({'name': name, 'exit_code': result.returncode,
                                   'log': log.name})
        if result.returncode != expected:
            raise ValueError(f'{name}: expected {expected}, got {result.returncode}; see {log}')
        return log.read_text()

    try:
        modes = [('c++20', 'arc', 'O0')] if full else [
            (standard, ownership, optimization)
            for standard in ('c++20', 'c++23')
            for ownership in ('arc', 'mrc') for optimization in ('O0', 'O2')]
        for standard, ownership, optimization in modes:
            label = '-'.join((standard, ownership, optimization))
            with tempfile.TemporaryDirectory(prefix='objc3-factory-package-') as temporary:
                work = Path(temporary)
                source = work / 'source'
                shutil.copytree(fixture, source)
                include = work / 'include'
                (include / 'objc3c').mkdir(parents=True)
                (include / 'objc3c/runtime').symlink_to(headers, target_is_directory=True)
                (include / 'objc3').symlink_to(ROOT / 'stdlib/include/objc3', target_is_directory=True)
                config = json.loads((source / 'provider/package.json').read_text())
                exports = config['exports'] if full else [
                    'NativeClosureFactories', 'NativeFactoryTypeProjections',
                    'NativeFactoryTemplateArguments', 'NativeGenericFactoryProjections',
                    'NativeFactoryValueArguments', 'NativeClosureResultProjections',
                    'NativeLambdaFactoryResults', 'NativeNestedFactoryTypes',
                    'NativeForeignAttributes', 'NativeCustomCoroutines', 'NativeDeclarationAttributes']
                built = work / 'built'
                module = built / 'modules' / MODULE
                module.mkdir(parents=True)
                interface = module / (MODULE + '.objc3i')
                provider = module / 'module.o'
                mode = ['--objc3', '--objcxx', '--runtime', 'gnustep-2.2',
                        '--target', 'x86_64-unknown-linux-gnu', '--sdk-root', str(foundation)]
                if ownership == 'arc':
                    mode.append('--arc')
                flags = ['--no-default-config', *instrumentation,
                         '-std=' + standard, '-' + optimization,
                         '-fblocks', '-fcoro-aligned-allocation', '-fexceptions',
                         '-fobjc-exceptions', '-fobjc-arc-exceptions',
                         '-fmodules', '-fmodules-cache-path=' + str(work / 'modules'),
                         '-I' + str(include), '-isystem', str(sdk / 'include')]
                if not full:
                    flags.append('-DOBJC3_FACTORY_PACKAGE_FOCUSED=1')
                if ownership == 'mrc':
                    flags.append('-fno-objc-arc')
                run(label + '-provider', [compiler, source / 'provider/module.mm',
                    *mode, '--emit-object', provider, '--emit-interface', interface,
                    '--module', MODULE, '--package', config['package'],
                    '--module-owned-names', '--module-source-root', source / 'provider',
                    *[arg for name in exports for arg in ('--export-declaration', name)],
                    '--', *flags])
                sidecar = interface.with_suffix('.objc3i.json')
                if not report['cases']:
                    shutil.copyfile(interface, output / 'emitted.objc3i')
                    shutil.copyfile(sidecar, output / 'emitted.objc3i.json')
                metadata = json.loads(sidecar.read_text())
                (check_cpp_metadata if full else check_factory_metadata)(metadata)
                symbols = run(label + '-symbols', ['nm', '--defined-only', provider])
                if not any(row.get('native_symbol_name', '') in symbols
                           for row in metadata['declarations']
                           if row['qualified_name'] == 'NativeNestedFactoryTypes::provider_owned'
                           and row.get('native_symbol_name')):
                    raise ValueError('factory provider ABI symbol missing')
                visibility = run(label + '-visibility', ['readelf', '-W', '-s', provider])
                packet_symbol = next(row.get('native_symbol_name', '')
                    for row in metadata['declarations']
                    if row['qualified_name'] == 'NativeDeclarationAttributes::provider_packet')
                if not packet_symbol or not any(packet_symbol in line and ' HIDDEN ' in line
                                                for line in visibility.splitlines()):
                    raise ValueError('native declaration attribute lost provider symbol visibility')
                manifest = {'schema': packages.SCHEMA, 'sdk_seal': development_lock,
                    'target': 'x86_64-unknown-linux-gnu', 'optimization': optimization,
                    'packages': [{'language': 'objective-c++', 'standard': standard,
                        'ownership': ownership, 'provider_ownership': ownership,
                        'exports': sorted(exports), 'module': metadata['module'],
                        'dependencies': [], 'artifacts': {
                            str(path.relative_to(built)): packages.digest(path)
                            for path in (provider, interface, sidecar)}}]}
                package = packages.copy_package(built, manifest, work / 'publication')
                identity = packages.content_hash(manifest)
                packages.install(package, identity, work / 'installed')
                installed = work / 'relocated packages'
                (work / 'installed').rename(installed)
                package = installed / identity
                packages.verify_package(package, identity)
                client = work / 'client'
                shutil.copytree(source / 'app', client)
                shutil.rmtree(source)
                shutil.rmtree(built)
                shutil.rmtree(work / 'publication')
                module = package / 'modules' / MODULE
                obj = work / 'client.o'
                imports = ['--module-search-path', str(module), '--import-module', MODULE,
                           metadata['module']['semantic_fingerprint'], config['package'],
                           '--private-import']
                if not eager:
                    imports.append('--source-imports')
                run(label + '-consumer', [compiler, client / 'main.mm', *mode,
                    '--emit-object', obj, *imports, '--', *flags])
                executable = work / 'witness'
                run(label + '-link', [foundation / 'bin/clang++', '--no-default-config',
                    *instrumentation, '-fuse-ld=lld', obj, module / 'module.o', runtime, objc_runtime,
                    '-pthread', '-Wl,-rpath,' + str(objc_runtime.parent), '-o', executable])
                run(label + '-execute', [executable])
                rejected = run(label + '-rejected', [compiler,
                    client / 'factory-template-rejected.mm', *mode, '--emit-object',
                    work / 'rejected.o', *imports, '--', *flags], expected=1)
                if (work / 'rejected.o').exists() or not all(
                        word in rejected for word in ('overloaded', 'variable template',
                                                       'requires template arguments')):
                    raise ValueError('unapplied factory template escaped native lookup checking')
                report['cases'].append({'name': label, 'provider_source_removed': True,
                    'package_sha256': identity, 'specialization_lookup_rejected': True})
        if inputs != {path: packages.digest(Path(path)) for path in inputs}:
            raise ValueError('compiler/runtime inputs changed during qualification')
        if source_inputs != {path: packages.digest(Path(path)) for path in source_inputs}:
            raise ValueError('source/header inputs changed during qualification')
        report['status'] = 'PASS'
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        packages.write_json(output / 'qualification.json', report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('sdk', 'compiler', 'foundation', 'runtime', 'objc-runtime', 'output'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--full', action='store_true')
    parser.add_argument('--eager', action='store_true')
    parser.add_argument('--sanitizer', choices=('address,undefined', 'thread'))
    parser.add_argument('--resource-root', type=Path)
    args = parser.parse_args(argv)
    report = qualify(args.sdk, args.compiler, args.foundation, args.runtime,
                     args.objc_runtime, args.output, full=args.full, eager=args.eager,
                     sanitizer=args.sanitizer, resource_root=args.resource_root)
    print(json.dumps({'status': report['status'], 'cases': len(report['cases']),
                      'commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
