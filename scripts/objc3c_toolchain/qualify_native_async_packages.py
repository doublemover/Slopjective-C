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
        for language, standard, ownership, optimization in configurations():
            label = '-'.join((language, standard, ownership, optimization))
            with case_workspace(output, label) as root:
                workspace = prepare(sdk, root / 'sources', language, standard, ownership,
                                    fixture='native-async-packages')
                built = root / 'built'
                first = build_graph(sdk, workspace, built, optimization)
                repeated = build_graph(sdk, workspace, built, optimization)
                if len(repeated['nodes']) != 2 or not all(
                        row['reused'] for row in repeated['nodes'].values()):
                    raise ValueError('native async package failed complete compile reuse')
                metadata_path = built / 'modules/fixture.NativeAsync/fixture.NativeAsync.objc3i.json'
                metadata = json.loads(metadata_path.read_text())
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
