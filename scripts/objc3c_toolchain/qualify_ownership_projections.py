"""Execute closed C ABI ownership projections through source and binary graphs."""
from __future__ import annotations

import argparse
from contextvars import copy_context
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import shutil
import sys
import tempfile

if __package__:
    from . import binary_packages as packages
    from .source_graph import build_graph
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from build import build_graph


def prepare(sdk: Path, root: Path, language: str, standard: str, ownership: str) -> Path:
    shutil.copytree(sdk / 'share/objc3c/examples/ownership-projection', root)
    suffix = '.mm' if language == 'objective-c++' else '.m'
    for path in root.glob('*/package.json'):
        config = json.loads(path.read_text())
        old = path.parent / config['source']
        source = old.with_suffix(suffix)
        if old != source:
            old.rename(source)
        provider = ('mrc' if ownership == 'arc' else 'arc') if config['name'] == 'fixture.Ownership' else ownership
        config.update(language=language, standard=standard, ownership=ownership,
                      provider_ownership=provider, source=source.name)
        path.write_text(json.dumps(config))
    workspace = root / 'workspace.json'
    config = json.loads(workspace.read_text())
    app = config['application']
    old = root / app['root'] / app['source']
    source = old.with_suffix(suffix)
    if old != source:
        old.rename(source)
    app.update(language=language, standard=standard, ownership=ownership, source=source.name)
    workspace.write_text(json.dumps(config))
    return workspace


def qualify_case(sdk: Path, output: Path, language: str, standard: str,
                 ownership: str, optimization: str) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    with tempfile.TemporaryDirectory(prefix="objc3 ownership 'outside checkout ") as temporary:
        root = Path(temporary)
        workspace = prepare(sdk, root / 'sources', language, standard, ownership)
        built = root / 'built'
        first = build_graph(sdk, workspace, built, optimization)
        repeat = build_graph(sdk, workspace, built, optimization)
        if not all(row['reused'] for row in repeat['nodes'].values()) or len(repeat['nodes']) != 3:
            raise ValueError('unchanged ownership projection did not reuse its exact three-node graph')
        module = first['modules']['fixture.Ownership']
        sidecar_path = built / 'modules/fixture.Ownership/fixture.Ownership.objc3i.json'
        sidecar = json.loads(sidecar_path.read_text())
        producer = json.loads(sidecar['ownership_projection'])
        if producer['module']['runtime_abi'] == module['runtime_abi'] or producer['ownership_projection']:
            raise ValueError('projection did not bind its original opposite-mode producer')
        publication = packages.publish(sdk, built, root / 'publication')
        installed = packages.install(Path(publication['package']), publication['content_sha256'], root / 'installed')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / publication['content_sha256']
        client = root / 'client'
        shutil.copytree(workspace.parent / 'app', client)
        config = json.loads(workspace.read_text())['application']
        application = client / 'application.json'
        application.write_text(json.dumps({'schema': packages.CONSUMER,
            **{key: config[key] for key in ('source', *packages.MODE, 'dependencies')}}))
        shutil.rmtree(workspace.parent)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        consumed = packages.consume(sdk, package, publication['content_sha256'], application, root / 'consumer')
        if consumed['provider_compilations'] or len(consumed['modules']) != 2:
            raise ValueError('source-free projected consumer rebuilt a provider or lost a re-export')
        packages.write_json(output / (label + '-consumer.json'), consumed)
        manifest_path = package / 'package.json'
        proof_path = package / 'modules/fixture.Ownership/fixture.Ownership.objc3i.json'
        original_manifest, original_sidecar = manifest_path.read_bytes(), proof_path.read_bytes()
        rejected = []
        for damage in ('unmarked-provider', 'wrong-producer-mode', 'wrong-producer-package',
                       'nested-proof', 'missing-proof', 'erased-capability', 'changed-contract', 'foreign-standard'):
            manifest, altered = json.loads(original_manifest), json.loads(original_sidecar)
            provider = json.loads(altered['ownership_projection'])
            row = next(row for row in manifest['packages'] if row['module']['canonical_name'] == 'fixture.Ownership')
            if damage == 'unmarked-provider':
                row['provider_ownership'] = ownership
            elif damage == 'wrong-producer-mode':
                provider['module']['runtime_abi'] = module['runtime_abi']
            elif damage == 'wrong-producer-package':
                provider['module']['package_identity'] = 'wrong-package'
            elif damage == 'nested-proof':
                provider['ownership_projection'] = 'nested'
            elif damage == 'erased-capability':
                altered['required_semantics'].remove('objc3.ownership-projection.v1')
            elif damage == 'changed-contract':
                provider['source_binding_sha256'] = '0' * 64
            elif damage == 'foreign-standard':
                provider['module']['language_profile'] = provider['module']['language_profile'].replace(
                    ';standard=' + standard + ';', ';standard=foreign;')
            altered['ownership_projection'] = '' if damage == 'missing-proof' else json.dumps(provider)
            proof_path.chmod(0o644)
            proof_path.write_text(json.dumps(altered))
            row['artifacts'][proof_path.relative_to(package).as_posix()] = packages.digest(proof_path)
            manifest_path.chmod(0o644)
            manifest_path.write_text(json.dumps(manifest))
            invalid = root / ('rejected-' + damage)
            try:
                packages.consume(sdk, package, packages.content_hash(manifest), application, invalid)
            except ValueError as error:
                if 'projection' not in str(error) and 'ownership' not in str(error):
                    raise ValueError('projection rejected for an unexpected reason: ' + damage) from error
                if invalid.exists():
                    raise ValueError('invalid ownership provenance reached compilation') from error
                rejected.append(damage)
            else:
                raise ValueError('invalid ownership provenance was accepted: ' + damage)
            finally:
                proof_path.write_bytes(original_sidecar)
                proof_path.chmod(0o444)
                manifest_path.write_bytes(original_manifest)
                manifest_path.chmod(0o444)
        return {'language': language, 'standard': standard, 'ownership': ownership,
                'provider_ownership': 'mrc' if ownership == 'arc' else 'arc',
                'optimization': optimization, 'sdk_seal': consumed['sdk_seal'],
                'projected_module': module, 'producer_module': producer['module'],
                'source_executions': 2, 'binary_executions': 1, 'reused_nodes': 3,
                'provider_compilations': 0, 'producer_sources_removed': True,
                'relocated': True, 'reexported': True, 'rejections': rejected}


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if jobs not in range(1, 5):
        raise ValueError('projection qualification jobs must be between 1 and 4')
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    modes = [('objective-c', 'c17'), ('objective-c++', 'c++20'), ('objective-c++', 'c++23')]
    configurations = [(language, standard, ownership, opt) for language, standard in modes
                      for ownership in ('arc', 'mrc') for opt in ('O0', 'O2')]
    cases, failures = [], []
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = [executor.submit(copy_context().run, qualify_case, sdk, output, *mode) for mode in configurations]
        for mode, future in zip(configurations, futures):
            try:
                cases.append(future.result())
            except Exception as error:
                failures.append({'configuration': mode, 'error': str(error)})
    fingerprints = {}
    for row in cases:
        mode = (row['language'], row['standard'], row['ownership'])
        current = row['projected_module']['semantic_fingerprint']
        if fingerprints.setdefault(mode, current) != current:
            failures.append({'configuration': mode, 'error': 'projection identity changed with optimization'})
    report = {'schema': 'objc3c.ownership-projection-qualification.v1',
              'status': 'FAIL' if failures else 'PASS', 'outside_checkout': True,
              'executions': sum(row['source_executions'] + row['binary_executions'] for row in cases),
              'source_executions': sum(row['source_executions'] for row in cases),
              'binary_executions': sum(row['binary_executions'] for row in cases),
              'rejections': sum(len(row['rejections']) for row in cases), 'cases': cases}
    if failures:
        report['failures'] = failures
    packages.write_json(receipt, report)
    if failures:
        raise ValueError('ownership projection qualification failed; see ' + str(receipt))
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=range(1, 5), default=2)
    args = parser.parse_args()
    sdk = args.sdk_root or Path(__file__).resolve().parents[3]
    report = qualify(sdk, args.output_dir, args.jobs)
    print(json.dumps({'status': report['status'], 'ownership_projection_executions': report['executions'],
                      'ownership_projection_rejections': report['rejections']}))


if __name__ == '__main__':
    main()
