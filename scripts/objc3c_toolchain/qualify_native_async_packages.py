"""Run native async effect imports through relocated source-free packages."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import sys

if __package__:
    from . import binary_packages as packages
    from .source_graph import build_graph
    from .qualify_native_async import configurations
    from .qualify_public_surfaces import prepare, case_workspace
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from build import build_graph
    from qualify_native_async import configurations
    from qualify_public_surfaces import prepare, case_workspace


def has_async(value):
    if isinstance(value, dict):
        return value.get('async') is True or any(map(has_async, value.values()))
    return isinstance(value, list) and any(map(has_async, value))


def has_checked_async(value):
    if isinstance(value, dict):
        return (value.get('async') is True and value.get('throws') is True
                and value.get('throws_kind') == 'typed') or any(
                    map(has_checked_async, value.values()))
    return isinstance(value, list) and any(map(has_checked_async, value))


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
    cases = []
    try:
        modes = [(*mode, 'native-async-packages', 'fixture.NativeAsync')
                 for mode in configurations()]
        modes += [('objective-c', 'c17', ownership, optimization,
                   'async-c-frame-packages', 'fixture.CFrame')
                  for ownership in ('arc', 'mrc') for optimization in ('O0', 'O2')]
        for language, standard, ownership, optimization, fixture, module in modes:
            label = '-'.join((language, standard, ownership, optimization))
            with case_workspace(output, label) as root:
                workspace = prepare(sdk, root / 'sources', language, standard, ownership,
                                    fixture=fixture)
                built = root / 'built'
                first = build_graph(sdk, workspace, built, optimization)
                repeated = build_graph(sdk, workspace, built, optimization)
                if len(repeated['nodes']) != 2 or not all(
                        row['reused'] for row in repeated['nodes'].values()):
                    raise ValueError('native async package failed complete compile reuse')
                metadata_path = built / 'modules' / module / (module + '.objc3i.json')
                metadata = json.loads(metadata_path.read_text())
                runtime_semantic = 'objc3.sdk-runtime-frames.v4.' + packages.digest(
                    sdk / 'include/objc3c/runtime/objc3_runtime_services.h')
                if runtime_semantic not in metadata['required_semantics']:
                    raise ValueError('native async interface lost its exact SDK frame contract')
                if language == 'objective-c++':
                    check_cpp_metadata(metadata)
                else:
                    names = {row['qualified_name']: row for row in metadata['declarations']}
                    if set(names) != {'c_frame_factory', 'c_frame_destroyed', 'c_frame_scoped'}:
                        raise ValueError('C frame interface lost its exported factories/scopes')
                    scope = names['c_frame_scoped']
                    effects = scope.get('effects', {})
                    if (not has_async(scope) or
                            'c-frame-v1' not in effects.get('async_abi_identity', '') or
                            'task-boundary:4' not in effects.get('actor_isolation_identity', '')):
                        raise ValueError('C frame scope lost its callable ABI or group contract')
                interface_sha = packages.digest(metadata_path)
                publication = packages.publish(sdk, built, root / 'publication')
                packages.install(Path(publication['package']), publication['content_sha256'],
                                 root / 'installed')
                relocated = root / 'relocated packages'
                (root / 'installed').rename(relocated)
                package = relocated / publication['content_sha256']
                client = root / 'client'
                shutil.copytree(workspace.parent / 'app', client)
                config = json.loads(workspace.read_text())['application']
                application = client / 'application.json'
                packages.write_json(application, {'schema': packages.CONSUMER,
                    **{key: config[key] for key in ('source', *packages.MODE, 'dependencies')}})
                shutil.rmtree(workspace.parent)
                shutil.rmtree(built)
                shutil.rmtree(root / 'publication')
                consumed = packages.consume(sdk, package, publication['content_sha256'],
                                             application, root / 'consumer')
                if consumed['provider_compilations'] or len(consumed['modules']) != 1:
                    raise ValueError('native async consumer rebuilt or lost its provider')
                cases.append({'language': language, 'standard': standard,
                    'ownership': ownership, 'optimization': optimization,
                    'provider_source_removed': True, 'provider_compilations': 0,
                    'runtime_semantic': runtime_semantic,
                    'interface_sha256': interface_sha, 'package_sha256': publication['content_sha256'],
                    'source_graph': first, 'reused_graph': repeated, 'consumer': consumed})
        report = {'schema': 'objc3c.native-async-packages.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'], 'executions': 3 * len(cases),
                  'cases': cases}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.native-async-packages.v1',
            'status': 'FAIL', 'sdk_seal': manifest['seal_sha256'], 'cases': cases,
            'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report


def check_cpp_metadata(metadata):
    if not has_async(metadata['declarations']):
        raise ValueError('native async interface erased the callable effect')
    if not has_checked_async(metadata['declarations']):
        raise ValueError('native async interface erased checked error identity')
    members = [row for row in metadata['declarations']
               if row.get('kind') == 'CXXMethod']
    if (not members or not has_checked_async(members)
            or 'objc3.cxx-member-interfaces.v1' not in metadata['required_semantics']
            or any(not row.get('native_symbol_name') for row in members)):
        raise ValueError('native async interface lost owned C++ member contracts')
    objc_methods = [row for row in metadata['declarations']
                    if row.get('kind') == 'ObjCMethod']
    if not has_checked_async(objc_methods):
        raise ValueError('native async interface lost Objective-C method effects')
    names = {row['qualified_name']: row for row in metadata['declarations']}
    scope = names.get('native_module_scoped', {})
    scope_effects = scope.get('effects', {})
    if (not has_async(scope) or
            'cxx-task-v1' not in scope_effects.get('async_abi_identity', '') or
            'task-boundary:4' not in scope_effects.get('actor_isolation_identity', '')):
        raise ValueError('C++ task scope lost its callable ABI or group contract')
    for name in ('native_module_frame', 'native_module_object_frame',
                 'native_module_block_frame'):
        if name not in names:
            raise ValueError('native async C frame factory missing: ' + name)
    for name in ('native_module_record', 'native_module_complex_value',
                 'native_module_data_value'):
        if name not in names or not has_checked_async(names[name]):
            raise ValueError('native async value interface lost checked identity: ' + name)
    for name in ('native_module_async_factory', 'native_module_checked_factory',
                 'native_module_async_block', 'native_module_checked_block'):
        if name not in names or not has_async(names[name]):
            raise ValueError('native async Block interface lost effects: ' + name)
    if not has_checked_async(names['native_module_checked_block']):
        raise ValueError('native async Block interface lost checked error identity')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root or Path(__file__).resolve().parents[3], args.output_dir)
    print(json.dumps({'status': report['status'], 'cases': len(report['cases']),
                      'executions': report['executions']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
