"""Qualify typed key paths through relocated source-free native packages."""
from __future__ import annotations

import argparse
from contextvars import copy_context
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import shutil
import subprocess
import sys

if __package__:
    from . import binary_packages as packages
    from .source_graph import build_graph
    from .sdk_example import sdk_verification
    from .qualify_public_surfaces import prepare, case_workspace
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from build import build_graph
    from sdk_example import sdk_verification
    from qualify_public_surfaces import prepare, case_workspace

MODULE = 'fixture.TypedKeyPaths'
SEMANTIC = 'objc3.typed-keypaths.v1'


def check_metadata(metadata: dict) -> None:
    if SEMANTIC not in metadata['required_semantics']:
        raise ValueError('typed key-path interface lost its required semantic')
    declarations = {row['qualified_name']: row for row in metadata['declarations']}
    for name in ('PackagePathRoot', 'PackagePathPair', 'PackagePathBits'):
        if name not in declarations:
            raise ValueError('typed key-path interface lost public type: ' + name)
    for name, writable in (('package_count_path', True),
                           ('package_nullable_path', True),
                           ('package_field_path', True),
                           ('package_bits_path', True),
                           ('package_identity_path', False),
                           ('package_narrowed_path', False)):
        identity = declarations.get(name, {}).get('canonical_type_identity', '')
        if ('KeyPath<' not in identity or
                ('WritableKeyPath<' in identity) != writable):
            raise ValueError('typed key-path interface lost root/value/writability: ' + name)


def configurations():
    for language, standard in (('objective-c', 'c17'),
                               ('objective-c++', 'c++20'),
                               ('objective-c++', 'c++23')):
        for ownership in ('arc', 'mrc'):
            for optimization in ('O0', 'O2'):
                yield language, standard, ownership, optimization


def qualify_case(sdk: Path, output: Path, language: str, standard: str,
                 ownership: str, optimization: str, fixtures: Path | None) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    with case_workspace(output, label) as root:
        workspace = prepare(sdk, root / 'sources', language, standard, ownership,
                            fixture='typed-keypath-packages', fixture_root=fixtures)
        built = root / 'built'
        first = build_graph(sdk, workspace, built, optimization)
        repeated = build_graph(sdk, workspace, built, optimization)
        if (len(repeated['nodes']) != 2 or
                not all(row['reused'] for row in repeated['nodes'].values())):
            raise ValueError('typed key-path package failed complete compile reuse')
        metadata_path = built / 'modules' / MODULE / (MODULE + '.objc3i.json')
        metadata = json.loads(metadata_path.read_text())
        check_metadata(metadata)
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
            raise ValueError('source-free typed key-path consumer rebuilt or lost its provider')
        # Importing a published readonly path must retain its native static
        # contract after the provider and original build tree are gone.
        consumer = root / 'consumer'
        original_object = packages.digest(consumer / 'application.o')
        client_source = client / config['source']
        client_source.write_text('@import objc3.keypath;\n@import fixture.TypedKeyPaths;\n'
            'void rejected_set(PackagePathRoot *root) {\n'
            '  KeyPathSet(package_identity_path(), root, 1);\n}\n')
        try:
            packages.consume(sdk, package, publication['content_sha256'],
                             application, consumer)
        except subprocess.CalledProcessError:
            log = consumer / 'command-0.log'
            text = log.read_text()
            if 'KeyPathSet requires a WritableKeyPath value' not in text:
                raise ValueError('readonly package rejection lost its native diagnostic')
            if ((consumer / 'consumer-report.json').exists() or
                    packages.digest(consumer / 'application.o') != original_object):
                raise ValueError('failed readonly consumer retained success or replaced its object')
            rejection_sha = packages.digest(log)
            shutil.copyfile(log, output / (label + '-readonly-rejected.log'))
        else:
            raise ValueError('source-free key-path import erased its readonly contract')
        return {'language': language, 'standard': standard, 'ownership': ownership,
                'optimization': optimization, 'provider_source_removed': True,
                'provider_compilations': 0, 'interface_sha256': interface_sha,
                'package_sha256': publication['content_sha256'],
                'readonly_rejection_log_sha256': rejection_sha,
                'source_graph': first, 'reused_graph': repeated, 'consumer': consumed}


def qualify(sdk: Path, output: Path, *, jobs: int = 1,
            fixtures: Path | None = None) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if fixtures is not None:
        fixtures = fixtures.resolve(strict=True)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    source = fixtures or sdk / 'share/objc3c/examples/typed-keypath-packages'
    before = {str(p.relative_to(source)): packages.digest(p)
              for p in sorted(source.rglob('*')) if p.is_file()}
    if not before:
        raise ValueError('typed key-path package fixture is empty')
    cases = []
    try:
        # A gate audits its frozen SDK at both boundaries. Independent user
        # build/publish/consume commands retain their ordinary full audits.
        with sdk_verification(sdk):
            with ThreadPoolExecutor(max_workers=max(1, min(jobs, 4))) as executor:
                futures = [executor.submit(copy_context().run, qualify_case, sdk,
                    output, *mode, fixtures) for mode in configurations()]
                for future in futures:
                    cases.append(future.result())
        manifest = json.loads((sdk / 'objc3-sdk.json').read_text())
        after = {str(p.relative_to(source)): packages.digest(p)
                 for p in sorted(source.rglob('*')) if p.is_file()}
        if after != before:
            raise ValueError('typed key-path package fixtures changed during qualification')
        report = {'schema': 'objc3c.typed-keypath-packages.v1', 'status': 'PASS',
                  'sdk_seal': manifest['seal_sha256'], 'executions': 3 * len(cases),
                  'cases': cases, 'rejections': len(cases), 'required_semantic': SEMANTIC,
                  'fixture_hashes': before}
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.typed-keypath-packages.v1',
            'status': 'FAIL', 'cases': cases, 'error': str(error)})
        raise
    packages.write_json(receipt, report)
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=1)
    parser.add_argument('--fixtures', type=Path)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root or Path(__file__).resolve().parents[3],
                     args.output_dir, jobs=args.jobs, fixtures=args.fixtures)
    print(json.dumps({'status': report['status'], 'cases': len(report['cases']),
                      'executions': report['executions']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
