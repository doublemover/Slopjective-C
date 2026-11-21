"""Qualify native public availability through source and immutable consumers."""
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
    with case_workspace(output, label) as root:
        workspace = prepare(sdk, root / 'sources', language, standard, ownership,
                            fixture='api-availability')
        manifest = workspace.parent / 'leaf/package.json'
        config = json.loads(manifest.read_text())
        if language != 'c++':
            config['exports'].append('AvailabilityRoot')
            packages.write_json(manifest, config)
        source = manifest.parent / config['source']
        built = root / 'built'
        first = build_graph(sdk, workspace, built, optimization)
        repeated = build_graph(sdk, workspace, built, optimization)
        if len(repeated['nodes']) != 3 or not all(row['reused'] for row in repeated['nodes'].values()):
            raise ValueError('availability graph failed complete compile reuse')
        source.write_text(source.read_text().replace('return 1; }', 'return 2; }'))
        edited = build_graph(sdk, workspace, built, optimization)
        if (edited['modules'] != first['modules'] or
                {name for name, row in edited['nodes'].items() if not row['reused']} != {'fixture.AvailabilityLeaf'}):
            raise ValueError('private edit changed availability identities or rebuilt importers')
        source.write_text(source.read_text().replace('use availability_value', 'choose availability_value'))
        evolved = build_graph(sdk, workspace, built, optimization)
        if (evolved['modules']['fixture.AvailabilityLeaf'] == first['modules']['fixture.AvailabilityLeaf'] or
                any(row['reused'] for row in evolved['nodes'].values())):
            raise ValueError('native availability edit did not invalidate dependent compilations')
        publication = packages.publish(sdk, built, root / 'publication')
        packages.install(Path(publication['package']), publication['content_sha256'], root / 'installed')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / publication['content_sha256']
        client = root / 'client'
        shutil.copytree(workspace.parent / 'app', client)
        app = json.loads(workspace.read_text())['application']
        application = client / 'application.json'
        packages.write_json(application, {'schema': packages.CONSUMER,
            **{key: app[key] for key in ('source', *packages.MODE, 'dependencies')}})
        shutil.rmtree(workspace.parent)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        consumer = root / 'consumer'
        consumed = packages.consume(sdk, package, publication['content_sha256'], application, consumer)
        if consumed['provider_compilations'] or len(consumed['modules']) != 2:
            raise ValueError('availability consumer rebuilt providers or lost native re-exports')
        packages.write_json(output / (label + '-consumer.json'), consumed)
        original_object = packages.digest(consumer / 'application.o')
        prefix = '#pragma clang diagnostic error "-Wdeprecated-declarations"\n'
        if language != 'c++': prefix += '@import fixture.AvailabilityTop;\n'
        probes = [
            ('deprecated-function', 'int main(void) { return availability_old(); }', 'choose availability_value'),
            ('unavailable-function', 'int main(void) { return availability_removed(); }', 'removed API'),
            ('deprecated-global', 'int main(void) { return availability_old_global; }', 'old global'),
            ('deprecated-alias', 'int main(void) { return (AvailabilityOldAlias)0; }', 'old alias'),
            ('deprecated-enumerator', 'int main(void) { return availability_old_choice; }', 'old choice'),
            ('deprecated-field', 'int main(void) { return ((AvailabilityValue*)0)->old_field; }', 'old field')]
        if language != 'c++':
            probes += [('deprecated-method', 'int probe(AvailabilityRoot *v) { return [v oldMethod]; }', 'old method'),
                       ('deprecated-property', 'int probe(AvailabilityRoot *v) { return v.oldProperty; }', 'old property')]
        rejected = []
        client_source = client / app['source']
        original_client = client_source.read_text()
        def failure(name, expected):
            try:
                packages.consume(sdk, package, publication['content_sha256'], application, consumer)
            except subprocess.CalledProcessError:
                log = consumer / 'command-0.log'
                text = log.read_text()
                if expected not in text or 'error:' not in text:
                    raise ValueError('availability rejection lacks its native diagnostic: ' + name)
                if (consumer / 'consumer-report.json').exists() or packages.digest(consumer / 'application.o') != original_object:
                    raise ValueError('availability rejection retained success or replaced prior object')
                target = output / (label + '-' + name + '.log')
                shutil.copyfile(log, target)
                rejected.append({'case': name, 'log_sha256': packages.digest(target)})
            else:
                raise ValueError('native availability contract was bypassed: ' + name)
        for name, code, message in probes:
            client_source.write_text(prefix + code + '\n')
            failure(name, message)
        client_source.write_text(original_client)
        module = package / 'modules/fixture.AvailabilityLeaf/fixture.AvailabilityLeaf.objc3i'
        sidecar = module.with_suffix(module.suffix + '.json')
        package_manifest = package / 'package.json'
        original_source, original_sidecar = module.read_bytes(), sidecar.read_bytes()
        original_manifest = package_manifest.read_bytes()
        attributes = json.loads(original_sidecar)
        if ('objc3.api-availability.v1' not in attributes['required_semantics'] or
                'platform API' not in original_source.decode()):
            raise ValueError('published interface lost availability metadata')
        for damage in ('altered-message', 'missing-capability'):
            data = json.loads(original_sidecar)
            if damage == 'altered-message':
                updated_source = original_source.replace(b'choose availability_value', b'forged availability contract')
                if updated_source == original_source: raise ValueError('tamper fixture failed to change native metadata')
                data['canonical_source_sha256'] = hashlib.sha256(updated_source).hexdigest()
            else:
                updated_source = original_source
                data['required_semantics'].remove('objc3.api-availability.v1')
            for path in (module, sidecar, package_manifest): path.chmod(0o644)
            module.write_bytes(updated_source)
            packages.write_json(sidecar, data)
            manifest_data = json.loads(original_manifest)
            row = next(row for row in manifest_data['packages'] if row['module']['canonical_name'] == 'fixture.AvailabilityLeaf')
            for path in (module, sidecar): row['artifacts'][path.relative_to(package).as_posix()] = packages.digest(path)
            packages.write_json(package_manifest, manifest_data)
            # Recompute package content so native semantic validation, rather
            # than an outer checksum mismatch, rejects the forged contract.
            forged = packages.content_hash(manifest_data)
            try:
                packages.consume(sdk, package, forged, application, consumer)
            except subprocess.CalledProcessError:
                log = consumer / 'command-0.log'; text = log.read_text()
                if 'O3IFC' not in text or (consumer / 'consumer-report.json').exists() or packages.digest(consumer / 'application.o') != original_object:
                    raise ValueError('forged availability was not rejected by native interface validation')
                target = output / (label + '-' + damage + '.log'); shutil.copyfile(log, target)
                rejected.append({'case': damage, 'native_interface_rejection': True, 'log_sha256': packages.digest(target)})
            else: raise ValueError('forged availability contract was accepted')
            finally:
                module.write_bytes(original_source); sidecar.write_bytes(original_sidecar); package_manifest.write_bytes(original_manifest)
                for path in (module, sidecar, package_manifest): path.chmod(0o444)
        return {'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization,
                'sdk_seal': consumed['sdk_seal'], 'module': evolved['modules']['fixture.AvailabilityLeaf'],
                'source_executions': 4, 'binary_executions': 1, 'reused_nodes': 3,
                'private_edit_rebuilt': ['fixture.AvailabilityLeaf'], 'metadata_edit_invalidated': 3,
                'provider_compilations': 0, 'producer_sources_removed': True, 'relocated': True,
                'reexported': True, 'rejected_object_preserved': True, 'failed_consumer_record_removed': True,
                'rejections': rejected}


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if jobs not in range(1, 5): raise ValueError('availability qualification jobs must be between 1 and 4')
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'; receipt.unlink(missing_ok=True)
    modes = [('objective-c', 'c17', mode) for mode in ('arc', 'mrc')]
    modes += [('objective-c++', standard, mode) for standard in ('c++20', 'c++23') for mode in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    configurations = [(*mode, opt) for mode in modes for opt in ('O0', 'O2')]
    cases, failures = [], []
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = [executor.submit(copy_context().run, qualify_case, sdk, output, *case) for case in configurations]
        for future in futures:
            try: cases.append(future.result())
            except Exception as error: failures.append(str(error))
    fingerprints = {}
    for row in cases:
        key = (row['language'], row['standard'], row['ownership'])
        fingerprint = row['module']['semantic_fingerprint']
        if fingerprints.setdefault(key, fingerprint) != fingerprint: failures.append('availability identity depends on optimization')
    report = {'schema': 'objc3c.api-availability-qualification.v1', 'status': 'FAIL' if failures else 'PASS',
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
    print(json.dumps({'status': report['status'], 'api_availability_executions': report['executions'],
                      'api_availability_rejections': report['rejections']}))
    return 0


if __name__ == '__main__': raise SystemExit(main())
