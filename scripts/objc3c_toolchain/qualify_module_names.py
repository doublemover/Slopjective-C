"""Link isolated native module names through relocated, source-free packages."""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
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

CAPABILITY = 'objc3.module-owned-names.v1'
NATIVE_NAME = re.compile(r'__objc3_m_[0-9a-f]{64}_[0-9a-f]{64}')


def qualify_case(sdk: Path, output: Path, language: str, standard: str,
                 ownership: str, optimization: str) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    with case_workspace(output, label) as root:
        workspace = prepare(sdk, root / 'sources', language, standard, ownership, fixture='module-names')
        built = root / 'built'
        first = build_graph(sdk, workspace, built, optimization)
        repeated = build_graph(sdk, workspace, built, optimization)
        if len(repeated['nodes']) != 3 or not all(row['reused'] for row in repeated['nodes'].values()):
            raise ValueError('owned-name graph failed complete reuse')
        left = workspace.parent / 'left'
        manifest = left / 'package.json'
        config = json.loads(manifest.read_text())
        source = left / config['source']
        source.write_text(source.read_text().replace('return 7;', 'return 8;'))
        private = build_graph(sdk, workspace, built, optimization)
        header = left / 'owned.h'
        header.write_text(header.read_text().replace('PRIVATE_BIAS 0', 'PRIVATE_BIAS 1'))
        edited_header = build_graph(sdk, workspace, built, optimization)
        for report in (private, edited_header):
            if (report['modules'] != first['modules'] or
                    {name for name, row in report['nodes'].items() if not row['reused']} != {'fixture.NamesLeft'}):
                raise ValueError('private source/header edit changed the interface or rebuilt unrelated nodes')
        config['package'] = 'names-left-v2'
        packages.write_json(manifest, config)
        config = json.loads(workspace.read_text())
        config['application']['dependencies'][0]['package'] = 'names-left-v2'
        packages.write_json(workspace, config)
        evolved = build_graph(sdk, workspace, built, optimization)
        if ({name for name, row in evolved['nodes'].items() if not row['reused']} != {'fixture.NamesLeft', '@application'} or
                evolved['modules']['fixture.NamesLeft'] == first['modules']['fixture.NamesLeft']):
            raise ValueError('package identity did not change native names and invalidate its importer')
        symbol_sets, symbol_evidence = [], {}
        for name in evolved['order']:
            directory = built / 'modules' / name
            metadata = json.loads((directory / (name + '.objc3i.json')).read_text())
            if CAPABILITY not in metadata['required_semantics']:
                raise ValueError('source package omitted its owned-name capability')
            symbols = subprocess.run([str(sdk / 'bin/llvm-readobj'), '--symbols', str(directory / 'module.o')],
                                     check=True, capture_output=True, text=True, timeout=30).stdout
            if any(d.get('native_symbol_name', '') not in symbols for d in metadata['declarations']):
                raise ValueError('native object omitted its exported module binding')
            names = set(NATIVE_NAME.findall(symbols))
            if len(names) < 2 or any(re.search(r'Name: ' + old + r'(?:\s|\()', symbols) for old in ('shared_helper', 'shared_global')):
                raise ValueError('private function/global names escaped module ownership')
            if language != 'c++' and '._OBJC_CLASS_RuntimeTwin' in symbols:
                raise ValueError('private Objective-C class retained its shared runtime name')
            if '++' in language and any(old in symbols for old in ('private_template', 'private_variable', 'private_namespace_helper', 'private_namespace_global')):
                raise ValueError('private template or namespace declaration retained its shared C++ symbol')
            symbol_sets.append(names)
            symbol_evidence[name] = {'sha256': hashlib.sha256(symbols.encode()).hexdigest(), 'owned_names': len(names)}
        if symbol_sets[0] & symbol_sets[1]:
            raise ValueError('separate providers share a native module name')
        publication = packages.publish(sdk, built, root / 'publication')
        packages.install(Path(publication['package']), publication['content_sha256'], root / 'installed')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / publication['content_sha256']
        client = root / 'client'
        shutil.copytree(workspace.parent / 'app', client)
        application = client / 'application.json'
        config = json.loads(workspace.read_text())['application']
        packages.write_json(application, {'schema': packages.CONSUMER,
            **{key: config[key] for key in ('source', *packages.MODE, 'dependencies')}})
        shutil.rmtree(workspace.parent)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        consumer = root / 'consumer'
        consumed = packages.consume(sdk, package, publication['content_sha256'], application, consumer)
        if consumed['provider_compilations'] or len(consumed['modules']) != 2:
            raise ValueError('relocated owned-name consumer rebuilt providers')
        packages.write_json(output / (label + '-consumer.json'), consumed)
        original_object = packages.digest(consumer / 'application.o')
        rejections = []

        def failure(name: str, content=publication['content_sha256'], *, semantic=False):
            try:
                packages.consume(sdk, package, content, application, consumer)
            except subprocess.CalledProcessError:
                log = consumer / 'command-0.log'
                diagnostics = log.read_text()
                if ('O3IFC' if semantic else 'error:') not in diagnostics:
                    raise ValueError('owned-name rejection lacks its native diagnostic: ' + name)
                if (consumer / 'consumer-report.json').exists() or packages.digest(consumer / 'application.o') != original_object:
                    raise ValueError('failed owned-name consumer retained success or replaced its previous object')
                target = output / (label + '-' + name + '.log')
                shutil.copyfile(log, target)
                rejections.append({'case': name, 'log_sha256': packages.digest(target)})
            else:
                raise ValueError('native module contract was bypassed: ' + name)

        client_source = client / config['source']
        original_client = client_source.read_text()
        prefix = '@import fixture.NamesLeft;\n@import fixture.NamesRight;\n' if language != 'c++' else ''
        probes = [('private-function', 'shared_helper()'), ('private-global', 'shared_global')]
        if language != 'c++':
            probes += [('private-class', 'sizeof(RuntimeTwin*)'), ('private-protocol', 'sizeof(id<RuntimeTwin>)')]
        for name, expression in probes:
            client_source.write_text(prefix + 'int main(void) { return ' + expression + '; }\n')
            failure(name)
        client_source.write_text(original_client)
        module = package / 'modules/fixture.NamesLeft/fixture.NamesLeft.objc3i'
        sidecar = module.with_suffix('.objc3i.json')
        manifest_path = package / 'package.json'
        original_source, original_sidecar, original_manifest = module.read_bytes(), sidecar.read_bytes(), manifest_path.read_bytes()
        for damage in ('forged-native-binding', 'removed-capability'):
            data = json.loads(original_sidecar)
            updated = original_source
            if damage == 'forged-native-binding':
                old = next(d['native_symbol_name'] for d in data['declarations'] if d.get('native_symbol_name'))
                updated = original_source.replace(old.encode(), b'__objc3_m_forged')
                if updated == original_source:
                    raise ValueError('owned-name damage probe changed no canonical binding')
                data['canonical_source_sha256'] = hashlib.sha256(updated).hexdigest()
            else:
                data['required_semantics'].remove(CAPABILITY)
            for path in (module, sidecar, manifest_path): path.chmod(0o644)
            try:
                module.write_bytes(updated)
                packages.write_json(sidecar, data)
                manifest = json.loads(original_manifest)
                row = next(row for row in manifest['packages'] if row['module']['canonical_name'] == 'fixture.NamesLeft')
                for path in (module, sidecar): row['artifacts'][path.relative_to(package).as_posix()] = packages.digest(path)
                packages.write_json(manifest_path, manifest)
                failure(damage, packages.content_hash(manifest), semantic=True)
            finally:
                module.write_bytes(original_source)
                sidecar.write_bytes(original_sidecar)
                manifest_path.write_bytes(original_manifest)
                for path in (module, sidecar, manifest_path): path.chmod(0o444)
        return {'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization,
                'sdk_seal': consumed['sdk_seal'], 'modules': evolved['modules'], 'symbols': symbol_evidence,
                'source_executions': 5, 'binary_executions': 1, 'reused_nodes': 3,
                'private_edit_rebuilt': ['fixture.NamesLeft'], 'package_edit_rebuilt': ['fixture.NamesLeft', '@application'],
                'provider_compilations': 0, 'producer_sources_removed': True, 'relocated': True,
                'rejected_object_preserved': True, 'failed_consumer_record_removed': True, 'rejections': rejections}


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if jobs not in range(1, 5): raise ValueError('owned-name qualification jobs must be between 1 and 4')
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    modes = [('objective-c', 'c17', mode) for mode in ('arc', 'mrc')]
    modes += [('objective-c++', standard, mode) for standard in ('c++20', 'c++23') for mode in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    cases, failures = [], []
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = [executor.submit(qualify_case, sdk, output, *mode, opt) for mode in modes for opt in ('O0', 'O2')]
        for future in futures:
            try: cases.append(future.result())
            except Exception as error: failures.append(str(error))
    fingerprints = {}
    for row in cases:
        key = (row['language'], row['standard'], row['ownership'])
        value = {name: module['semantic_fingerprint'] for name, module in row['modules'].items()}
        if fingerprints.setdefault(key, value) != value:
            failures.append('owned-name identity depends on optimization')
    report = {'schema': 'objc3c.module-names-qualification.v1', 'status': 'FAIL' if failures else 'PASS',
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
    print(json.dumps({'status': report['status'], 'module_names_executions': report['executions'],
                      'module_names_rejections': report['rejections']}))
    return 0


if __name__ == '__main__': raise SystemExit(main())
