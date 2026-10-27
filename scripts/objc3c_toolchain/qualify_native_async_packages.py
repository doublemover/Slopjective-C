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
               if row.get('kind') in ('CXXMethod', 'CXXConstructor',
                                      'CXXDestructor', 'CXXConversion')]
    if (not members or not has_checked_async(members)
            or 'objc3.cxx-member-interfaces.v1' not in metadata['required_semantics']
            or any(not row.get('native_symbol_name') for row in members
                   if ';deleted=1' not in row.get('canonical_type_identity', ''))):
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
    if 'objc3.cxx-class-lifecycles.v1' not in metadata['required_semantics']:
        raise ValueError('nontrivial C++ class package lost its lifecycle capability')
    for kind in ('CXXConstructor', 'CXXDestructor', 'CXXConversion'):
        if not any(row['kind'] == kind for row in metadata['declarations']):
            raise ValueError('nontrivial C++ class package lost declaration kind: ' + kind)
    for name in ('NativeClassValue', 'NativeMoveOnly', 'NativeDeletedMember'):
        if ';cxx-class-lifecycle:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('class package lost structural lifecycle identity: ' + name)
    deleted_member = names.get('NativeDeletedMember::number', {})
    if ';deleted=1' not in deleted_member.get('canonical_type_identity', ''):
        raise ValueError('deleted aggregate member lost its unavailable declaration identity')
    if 'objc3.cxx-class-structure.v1' not in metadata['required_semantics']:
        raise ValueError('C++ class package lost its base/defaulting capability')
    for name in ('NativeAggregate', 'NativeDefaultedValue', 'NativeDerivedValue', 'NativeAccessValue'):
        identity = names.get(name, {}).get('canonical_type_identity', '')
        if ';cxx-class-structure:v1;' not in identity or ';base=' not in identity:
            raise ValueError('C++ class package lost base layout identity: ' + name)
    if not any(';defaulted=1;' in row.get('canonical_type_identity', '')
               for row in metadata['declarations']):
        raise ValueError('C++ class package lost defaulted special member identity')
    access = names['NativeAccessValue']['canonical_type_identity']
    if ':access=private' not in access or ':access=protected' not in access:
        raise ValueError('C++ class package lost private/protected base access')
    if 'objc3.cxx-class-dispatch.v1' not in metadata['required_semantics']:
        raise ValueError('C++ class package lost its virtual dispatch capability')
    for name in ('NativeVirtualRoot', 'NativeVirtualLeft', 'NativeVirtualRight', 'NativeVirtualDiamond'):
        row = names.get(name, {})
        identity = row.get('canonical_type_identity', '')
        if (';cxx-class-dispatch:v1;' not in identity or
                not row.get('native_symbol_name', '').startswith('__objc3_m_')):
            raise ValueError('C++ class package lost dispatch/type ownership: ' + name)
    diamond = names['NativeVirtualDiamond']['canonical_type_identity']
    if ';vbase=' not in diamond or ';final=1' not in diamond:
        raise ValueError('C++ class package lost shared virtual base/final identity')
    root_score = names.get('NativeVirtualRoot::score', {}).get('canonical_type_identity', '')
    final_score = names.get('NativeVirtualDiamond::score', {}).get('canonical_type_identity', '')
    if ';pure=1' not in root_score or ';override=' not in final_score or ';final=1' not in final_score:
        raise ValueError('C++ class package lost pure/override/final member contracts')
    if 'objc3.cxx-namespaces.v1' not in metadata['required_semantics']:
        raise ValueError('C++ package lost its named namespace capability')
    for name in ('NativePublished', 'NativePublished::v1', 'NativePublished::v1::Value',
                 'NativePublished::v1::frame', 'NativeOther::Scalar'):
        if ';cxx-namespaces:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('C++ package lost namespace/inline scope identity: ' + name)
    aliases = [names.get(name, {}).get('stable_id')
               for name in ('NativePublished::v1::Scalar', 'NativeOther::Scalar')]
    if not all(aliases) or aliases[0] == aliases[1]:
        raise ValueError('C++ namespace aliases lost distinct native declaration identities')
    if 'objc3.cxx-nested-types.v1' not in metadata['required_semantics']:
        raise ValueError('C++ package lost its nested type capability')
    for name in ('NativeFamily','NativeFamily::Value','NativeFamily::Kind','NativeFamily::Result'):
        if ';cxx-nested-types:v1;' not in names.get(name,{}).get('canonical_type_identity',''):
            raise ValueError('C++ package lost nested type identity: '+name)
    if ';access=private' not in names.get('NativeFamily::Hidden',{}).get('canonical_type_identity',''):
        raise ValueError('C++ package lost private nested access')
    if 'objc3.cxx-using.v1' not in metadata['required_semantics']:
        raise ValueError('C++ package lost its using capability')
    if not any(row['kind']=='UsingDirective' for row in metadata['declarations']):
        raise ValueError('C++ package lost its using-directive')
    for semantic, name, marker in (
        ('objc3.cxx-block-expressions.v1', 'native_published_block', ';cxx-block-expressions:v1;'),
        ('objc3.cxx-statement-expressions.v1', 'native_published_statement', ';cxx-statement-expressions:v1;'),
        ('objc3.cxx-local-types.v1', 'native_published_local_type', ';cxx-local-types:v1;'),
    ):
        if semantic not in metadata.get('required_semantics', []):
            raise AssertionError('missing local expression capability: ' + semantic)
        if marker not in names.get(name, {}).get('canonical_type_identity', ''):
            raise AssertionError('missing local expression identity: ' + name)
    if 'objc3.cxx-friends.v1' not in metadata.get('required_semantics', []):
        raise AssertionError('missing C++ friend capability')
    for name in ('NativeFriendValue', 'native_friend_reveal', 'native_friend_external', 'native_friend_delayed'):
        if ';cxx-friends:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise AssertionError('missing C++ friend identity: ' + name)
    if 'objc3.cxx-thread-local.v1' not in metadata.get('required_semantics', []):
        raise AssertionError('missing C++ thread-local capability')
    for name in ('native_thread_value', 'native_inline_thread_value', 'NativeThreadState', 'native_local_thread_value'):
        if ';cxx-thread-local:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise AssertionError('missing C++ thread-local identity: ' + name)
    if 'objc3.cxx-lambda-expressions.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its lambda-expression capability')
    for name in ('native_closure', 'NativeClosureDefaults', 'native_closure_frame'):
        if ';cxx-lambda-expressions:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('C++ package lost lambda-expression identity: ' + name)
    if 'objc3.cxx-namespace-attributes.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its namespace attribute capability')
    for name in ('NativeHiddenScope', 'NativeVersioned::v2'):
        if ';cxx-namespace-attributes:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('C++ package lost namespace attribute identity: ' + name)
    if 'objc3.cxx-immediate-defaulting.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its immediate defaulting capability')
    for name in ('NativeImmediate', 'NativeImmediateOut'):
        if ';cxx-immediate-defaulting:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('C++ package lost record-owned immediate defaulting: ' + name)
    if 'objc3.cxx-out-of-line-defaulting.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its out-of-line defaulting capability')
    if ';cxx-out-of-line-defaulting:v1;' not in names.get('NativeOutOfLineDefaulted', {}).get('canonical_type_identity', ''):
        raise ValueError('C++ package lost record-owned out-of-line defaulting identity')
    if 'objc3.cxx-defaulted-comparisons.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its defaulted-comparison capability')
    for name in ('NativeComparable', 'NativeFriendComparable', 'NativeDerivedComparable'):
        if ';cxx-defaulted-comparisons:v1;' not in names.get(name, {}).get('canonical_type_identity', ''):
            raise ValueError('C++ package lost record-owned defaulted comparisons: ' + name)
    if 'objc3.cxx-definitions.v1' not in metadata.get('required_semantics', []):
        raise ValueError('C++ package lost its inline/constexpr definition capability')
    if ';cxx-definitions:v1;' not in names.get('NativeInline::Value', {}).get('canonical_type_identity', ''):
        raise ValueError('C++ package lost its record-owned inline definition identity')
    if 'objc3.cxx-declaration-expressions.v1' not in metadata.get('required_semantics', []):
        raise ValueError('missing C++ declaration-expression capability')
    if ';cxx-declaration-expressions:v1;' not in names.get('NativeDefaults::Object', {}).get('canonical_type_identity', ''):
        raise ValueError('missing record-owned declaration-expression identity')
    if ';cxx-using:v1;' not in names.get('NativeInherited',{}).get('canonical_type_identity',''):
        raise ValueError('C++ package lost member using/inherited constructor identity')
    for name in ('native_module_reference', 'NativeModuleWorker::frame_borrow'):
        declaration = names.get(name, {})
        if (not has_async(declaration) or
                'c-frame-v1' not in declaration.get('effects', {}).get('async_abi_identity', '') or
                not declaration.get('native_symbol_name')):
            raise ValueError('native borrowed frame lost its callable/member identity: ' + name)
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
