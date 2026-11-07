"""Qualify explicit native SPI grants through source and immutable packages."""
from __future__ import annotations

import argparse
from contextvars import copy_context
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

if __package__:
    from . import binary_packages as packages
    from .source_graph import build_graph
    from .qualify_public_surfaces import prepare, case_workspace
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from build import build_graph
    from qualify_public_surfaces import prepare, case_workspace


def qualify_case(sdk: Path, output: Path, language: str, standard: str,
                 ownership: str, optimization: str) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    objc = language != 'c++'
    with case_workspace(output, label) as root:
        workspace = prepare(sdk, root / 'sources', language, standard, ownership, fixture='module-spi')
        leaf_manifest = workspace.parent / 'leaf/package.json'
        leaf = json.loads(leaf_manifest.read_text())
        if objc:
            leaf['exports'] += ['SPIRoot', 'SPIProtocol']
            packages.write_json(leaf_manifest, leaf)
        leaf_source = leaf_manifest.parent / leaf['source']
        built = root / 'built'
        cold = build_graph(sdk, workspace, built, optimization)
        repeated = build_graph(sdk, workspace, built, optimization)
        if len(repeated['nodes']) != 3 or not all(row['reused'] for row in repeated['nodes'].values()):
            raise ValueError('SPI source graph failed complete reuse')
        leaf_source.write_text(leaf_source.read_text().replace('return 1; }', 'return 2; }'))
        private = build_graph(sdk, workspace, built, optimization)
        if private['modules'] != cold['modules'] or {name for name, row in private['nodes'].items() if not row['reused']} != {'fixture.SPILeaf'}:
            raise ValueError('private SPI implementation edit changed its API or rebuilt importers')
        config = json.loads(workspace.read_text())
        app = config['application']
        direct_grant = {'name': 'fixture.SPILeaf', 'package': 'spi-leaf-v1', 'visibility': 'private', 'spi': ['Testing']}
        app['dependencies'].append(direct_grant)
        packages.write_json(workspace, config)
        client_source = workspace.parent / app['root'] / app['source']
        original_client = client_source.read_text()
        qualified_prefix = '@import fixture.SPITop;\n@import fixture.SPILeaf;\n' if objc else ''
        privileged_client = qualified_prefix + 'int main(void) { return spi_answer() == 42 && spi_secret() == 22 && spi_global == 22 && spi_choice == 22 ? 0 : 1; }\n'
        client_source.write_text(privileged_client)
        granted = build_graph(sdk, workspace, built, optimization)
        if {name for name, row in granted['nodes'].items() if not row['reused']} != {'@application'}:
            raise ValueError('changing a direct SPI grant did not invalidate exactly its importer')
        leaf_source.write_text(leaf_source.read_text().replace('SPI(Testing)', 'SPI(Diagnostics)'))
        for path in (workspace, workspace.parent / 'top/package.json'):
            data = json.loads(path.read_text())
            edges = data['application']['dependencies'] if path == workspace else data['dependencies']
            for edge in edges:
                if 'spi' in edge: edge['spi'] = ['Diagnostics']
            packages.write_json(path, data)
        evolved = build_graph(sdk, workspace, built, optimization)
        if (evolved['modules']['fixture.SPILeaf'] == cold['modules']['fixture.SPILeaf'] or
                any(row['reused'] for row in evolved['nodes'].values())):
            raise ValueError('SPI partition edit failed to change identities and invalidate the graph')
        publication = packages.publish(sdk, built, root / 'publication')
        packages.install(Path(publication['package']), publication['content_sha256'], root / 'installed')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / publication['content_sha256']
        client = root / 'client'
        shutil.copytree(client_source.parent, client)
        application = client / 'application.json'
        config = json.loads(workspace.read_text())['application']
        config = {'schema': packages.CONSUMER, **{key: config[key] for key in ('source', *packages.MODE, 'dependencies')}}
        granted_config = json.loads(json.dumps(config))
        config['dependencies'] = [edge for edge in config['dependencies'] if edge['name'] != 'fixture.SPILeaf']
        packages.write_json(application, config)
        client_source = client / config['source']
        client_source.write_text(original_client)
        shutil.rmtree(workspace.parent)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        consumer = root / 'consumer'
        public = packages.consume(sdk, package, publication['content_sha256'], application, consumer)
        if public['provider_compilations'] or len(public['modules']) != 2:
            raise ValueError('SPI public binary consumer rebuilt providers or lost public re-exports')
        packages.write_json(output / (label + '-public-consumer.json'), public)
        original_object = packages.digest(consumer / 'application.o')
        rejections = []

        def failure(name: str, content: str = publication['content_sha256'], *, native_code: str | None = None):
            try:
                packages.consume(sdk, package, content, application, consumer)
            except subprocess.CalledProcessError:
                log = consumer / 'command-0.log'
                if (native_code or 'error:') not in log.read_text():
                    raise ValueError('SPI rejection lacks a native compiler diagnostic: ' + name)
                if (consumer / 'consumer-report.json').exists() or packages.digest(consumer / 'application.o') != original_object:
                    raise ValueError('SPI rejection retained success or replaced the previous object')
                target = output / (label + '-' + name + '.log')
                shutil.copyfile(log, target)
                rejections.append({'case': name, 'log_sha256': packages.digest(target)})
            else:
                raise ValueError('native SPI boundary was bypassed: ' + name)

        prefix = '@import fixture.SPITop;\n' if objc else ''
        probes = [('function', 'spi_secret()'), ('global', 'spi_global'), ('record', 'sizeof(struct SPIValue)'),
                  ('typedef', '(SPIAlias)0'), ('enumerator', 'spi_choice'), ('other-group', 'spi_other()')]
        if objc:
            probes += [('class', 'sizeof(SPIRoot*)'), ('protocol', 'sizeof(id<SPIProtocol>)')]
        for name, expression in probes:
            client_source.write_text(prefix + 'int main(void) { return ' + expression + '; }\n')
            failure('public-' + name)
        if objc:
            client_source.write_text(prefix + '#pragma clang diagnostic error "-Wobjc-method-access"\nint probe(id v) { return [v onlySecret]; }\nint main(void) { return 0; }\n')
            failure('public-selector')
            for name in ('spi_secret', 'spi_global', 'spi_choice'):
                client_source.write_text(prefix + 'int main(void) { return @fixture.SPILeaf.' + name + ('();' if name == 'spi_secret' else ';') + ' }\n')
                failure('qualified-' + name)
        packages.write_json(application, granted_config)
        client_source.write_text(privileged_client)
        privileged = packages.consume(sdk, package, publication['content_sha256'], application, consumer)
        if privileged['provider_compilations'] or len(privileged['modules']) != 2:
            raise ValueError('privileged binary consumer rebuilt providers')
        packages.write_json(output / (label + '-privileged-consumer.json'), privileged)
        original_object = packages.digest(consumer / 'application.o')
        client_source.write_text(qualified_prefix + 'int main(void) { return spi_other(); }\n')
        failure('granted-other-group')
        client_source.write_text(privileged_client)
        missing = json.loads(json.dumps(granted_config))
        next(edge for edge in missing['dependencies'] if edge['name'] == 'fixture.SPILeaf')['spi'] = ['Missing']
        packages.write_json(application, missing)
        failure('unknown-provider-group', native_code='O3IFC159')
        packages.write_json(application, config)
        failure('revoked-grant')
        packages.write_json(application, granted_config)
        module = package / 'modules/fixture.SPILeaf/fixture.SPILeaf.objc3i'
        sidecar = module.with_suffix(module.suffix + '.json')
        manifest_path = package / 'package.json'
        original_source, original_sidecar, original_manifest = module.read_bytes(), sidecar.read_bytes(), manifest_path.read_bytes()
        metadata = json.loads(original_sidecar)
        if 'objc3.api-partitions.v1' not in metadata['required_semantics'] or not any(d.get('spi_group') == 'Diagnostics' for d in metadata['declarations']):
            raise ValueError('published interface lost its native SPI partitions')
        for damage in ('altered-native-partition', 'removed-capability'):
            data = json.loads(original_sidecar)
            updated = original_source
            if damage == 'altered-native-partition':
                updated = original_source.replace(b'objc3.spi:Diagnostics', b'objc3.spi:Other')
                if updated == original_source: raise ValueError('SPI damage probe changed no native annotation')
                data['canonical_source_sha256'] = hashlib.sha256(updated).hexdigest()
            else:
                data['required_semantics'].remove('objc3.api-partitions.v1')
            for path in (module, sidecar, manifest_path): path.chmod(0o644)
            try:
                module.write_bytes(updated)
                packages.write_json(sidecar, data)
                manifest = json.loads(original_manifest)
                row = next(row for row in manifest['packages'] if row['module']['canonical_name'] == 'fixture.SPILeaf')
                for path in (module, sidecar): row['artifacts'][path.relative_to(package).as_posix()] = packages.digest(path)
                packages.write_json(manifest_path, manifest)
                failure(damage, packages.content_hash(manifest), native_code='O3IFC')
                if 'O3IFC' not in (consumer / 'command-0.log').read_text():
                    raise ValueError('SPI damage was not rejected by native semantic validation')
                rejections[-1]['native_interface_rejection'] = True
            finally:
                module.write_bytes(original_source)
                sidecar.write_bytes(original_sidecar)
                manifest_path.write_bytes(original_manifest)
                for path in (module, sidecar, manifest_path): path.chmod(0o444)
        return {'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization,
                'sdk_seal': privileged['sdk_seal'], 'module': evolved['modules']['fixture.SPILeaf'],
                'source_executions': 5, 'binary_executions': 2, 'reused_nodes': 3,
                'grant_edit_rebuilt': ['@application'], 'partition_edit_invalidated': 3,
                'provider_compilations': 0, 'producer_sources_removed': True, 'relocated': True,
                'reexports_do_not_forward_grants': True, 'rejected_object_preserved': True,
                'failed_consumer_record_removed': True, 'rejections': rejections}


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if jobs not in range(1, 5): raise ValueError('SPI qualification jobs must be between 1 and 4')
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    modes = [('objective-c', 'c17', mode) for mode in ('arc', 'mrc')]
    modes += [('objective-c++', standard, mode) for standard in ('c++20', 'c++23') for mode in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    cases, failures = [], []
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = [executor.submit(copy_context().run, qualify_case, sdk, output, *mode, opt) for mode in modes for opt in ('O0', 'O2')]
        for future in futures:
            try: cases.append(future.result())
            except Exception as error: failures.append(str(error))
    fingerprints = {}
    for row in cases:
        key = (row['language'], row['standard'], row['ownership'])
        fingerprint = row['module']['semantic_fingerprint']
        if fingerprints.setdefault(key, fingerprint) != fingerprint:
            failures.append('SPI identity depends on optimization')
    report = {'schema': 'objc3c.module-spi-qualification.v1', 'status': 'FAIL' if failures else 'PASS',
              'cases': cases, 'executions': sum(row['source_executions'] + row['binary_executions'] for row in cases),
              'rejections': sum(len(row['rejections']) for row in cases), 'failures': failures}
    packages.write_json(receipt, report)
    if failures: raise ValueError('; '.join(failures))
    return report


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=2)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root or Path(__file__).resolve().parents[3], args.output_dir, args.jobs)
    print(json.dumps({'status': report['status'], 'module_spi_executions': report['executions'],
                      'module_spi_rejections': report['rejections']}))
    return 0


if __name__ == '__main__': raise SystemExit(main())
