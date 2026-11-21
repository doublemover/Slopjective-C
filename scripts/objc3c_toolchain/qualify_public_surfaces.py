"""Qualify selected native public APIs through source and immutable packages."""
from __future__ import annotations

import argparse
from contextvars import copy_context
from concurrent.futures import ThreadPoolExecutor
from contextlib import contextmanager
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

if __package__:
    from . import binary_packages as packages
    from .source_graph import build_graph
else:
    sys.dont_write_bytecode = True
    import binary_packages as packages
    from build import build_graph


def prepare(sdk: Path, root: Path, language: str, standard: str, ownership: str, *,
            fixture: str = "public-module-surface", fixture_root: Path | None = None) -> Path:
    shutil.copytree(fixture_root if fixture_root is not None else
                    sdk / "share/objc3c/examples" / fixture, root)
    suffix = {'objective-c': '.m', 'objective-c++': '.mm', 'c++': '.cpp'}[language]
    for path in root.glob('*/package.json'):
        config = json.loads(path.read_text())
        old = path.parent / config['source']
        source = old.with_suffix(suffix)
        if old != source:
            old.rename(source)
        config.update(language=language, standard=standard, ownership=ownership,
                      provider_ownership=ownership, source=source.name)
        packages.write_json(path, config)
    path = root / 'workspace.json'
    config = json.loads(path.read_text())
    app = config['application']
    old = root / app['root'] / app['source']
    source = old.with_suffix(suffix)
    if old != source:
        old.rename(source)
    app.update(language=language, standard=standard, ownership=ownership, source=source.name)
    packages.write_json(path, config)
    return path


@contextmanager
def case_workspace(output: Path, label: str):
    with tempfile.TemporaryDirectory(prefix="objc3 selected API 'outside checkout ") as temporary:
        root = Path(temporary)
        try:
            yield root
        except Exception:
            retained = output / (label + '-failure')
            for log in root.rglob('*.log'):
                target = retained / log.relative_to(root)
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(log, target)
            raise


def qualify_case(sdk: Path, output: Path, language: str, standard: str,
                 ownership: str, optimization: str) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    with case_workspace(output, label) as root:
        workspace = prepare(sdk, root / 'sources', language, standard, ownership)
        built = root / 'built'
        first = build_graph(sdk, workspace, built, optimization)
        repeated = build_graph(sdk, workspace, built, optimization)
        if len(repeated['nodes']) != 3 or not all(row['reused'] for row in repeated['nodes'].values()):
            raise ValueError('selected API graph did not reuse all three nodes')
        leaf = first['modules']['fixture.SurfaceLeaf']
        source = workspace.parent / 'leaf' / json.loads((workspace.parent / 'leaf/package.json').read_text())['source']
        source.write_text(source.read_text().replace('return 1; }', 'return 2; }'))
        edited = build_graph(sdk, workspace, built, optimization)
        if (edited['modules'] != first['modules'] or
                {name for name, row in edited['nodes'].items() if not row['reused']} != {'fixture.SurfaceLeaf'}):
            raise ValueError('private implementation edit changed public identity or rebuilt importers')
        publication = packages.publish(sdk, built, root / 'publication')
        installed = packages.install(Path(publication['package']), publication['content_sha256'], root / 'installed')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / publication['content_sha256']
        # Expanding the public API must invalidate the complete dependent graph.
        path = workspace.parent / 'leaf/package.json'
        config = json.loads(path.read_text())
        config['exports'].append('surface_hidden')
        packages.write_json(path, config)
        expanded = build_graph(sdk, workspace, built, optimization)
        if (expanded['modules']['fixture.SurfaceLeaf'] == leaf or
                any(row['reused'] for row in expanded['nodes'].values())):
            raise ValueError('export-policy expansion did not invalidate every dependent node')
        client = root / 'client'
        shutil.copytree(workspace.parent / 'app', client)
        app = json.loads(workspace.read_text())['application']
        application = client / 'application.json'
        packages.write_json(application, {'schema': packages.CONSUMER,
            **{key: app[key] for key in ('source', *packages.MODE, 'dependencies')}})
        shutil.rmtree(workspace.parent)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        consumed = packages.consume(sdk, package, publication['content_sha256'], application, root / 'consumer')
        if consumed['provider_compilations'] or len(consumed['modules']) != 2:
            raise ValueError('source-free selected consumer rebuilt providers or lost its re-export')
        packages.write_json(output / (label + '-consumer.json'), consumed)
        rejected = []
        manifest_path = package / 'package.json'
        original = manifest_path.read_bytes()
        for damage in ('missing-root', 'extra-private-root', 'duplicate', 'empty', 'missing-field', 'old-schema'):
            manifest = json.loads(original)
            row = next(row for row in manifest['packages'] if row['module']['canonical_name'] == 'fixture.SurfaceLeaf')
            if damage == 'missing-root': row['exports'] = ['surface_value']
            elif damage == 'extra-private-root': row['exports'] = sorted([*row['exports'], 'surface_hidden'])
            elif damage == 'duplicate': row['exports'].append('surface_value')
            elif damage == 'empty': row['exports'] = []
            elif damage == 'missing-field': del row['exports']
            else: manifest['schema'] = 'objc3c.binary-package.v2'
            manifest_path.chmod(0o644)
            packages.write_json(manifest_path, manifest)
            bad_output = root / ('rejected-' + damage)
            try:
                packages.consume(sdk, package, packages.content_hash(manifest), application, bad_output)
            except ValueError:
                if bad_output.exists():
                    raise ValueError('invalid export metadata reached compilation')
                rejected.append({'case': damage, 'before_compilation': True})
            else:
                raise ValueError('altered export policy was accepted: ' + damage)
            finally:
                manifest_path.write_bytes(original)
                manifest_path.chmod(0o444)
        client_source = client / app['source']
        for damage in ('private-function', 'private-type'):
            prefix = '@import fixture.SurfaceTop;\n' if language != 'c++' else ''
            expression = ('@fixture.SurfaceLeaf.surface_hidden()' if language != 'c++' else 'surface_hidden()')
            if damage == 'private-type': expression = 'sizeof(struct HiddenSurface)'
            client_source.write_text(prefix + 'int main(void) { return ' + expression + '; }\n')
            bad_output = root / ('rejected-' + damage)
            try:
                packages.consume(sdk, package, publication['content_sha256'], application, bad_output)
            except subprocess.CalledProcessError:
                log = bad_output / 'command-0.log'
                expected = 'HiddenSurface' if damage == 'private-type' else 'surface_hidden'
                if expected not in log.read_text() or (bad_output / 'consumer-report.json').exists():
                    raise ValueError('private API rejection lacks its native diagnostic or retained stale success')
                shutil.copyfile(log, output / (label + '-' + damage + '.log'))
                rejected.append({'case': damage, 'native_diagnostic': True})
            else:
                raise ValueError('private module declaration leaked: ' + damage)
        return {'language': language, 'standard': standard, 'ownership': ownership,
                'optimization': optimization, 'sdk_seal': consumed['sdk_seal'], 'module': leaf,
                'source_executions': 4, 'binary_executions': 1,
                'reused_nodes': 3, 'private_edit_rebuilt': ['fixture.SurfaceLeaf'],
                'expanded_policy_invalidated': 3, 'provider_compilations': 0,
                'producer_sources_removed': True, 'relocated': True, 'reexported': True,
                'rejections': rejected}


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    if jobs not in range(1, 5):
        raise ValueError('public surface qualification jobs must be between 1 and 4')
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
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
        value = row['module']['semantic_fingerprint']
        if fingerprints.setdefault(key, value) != value:
            failures.append('public fingerprint depends on optimization')
    report = {'schema': 'objc3c.public-surface-qualification.v1',
              'status': 'FAIL' if failures else 'PASS', 'cases': cases,
              'executions': sum(row['source_executions'] + row['binary_executions'] for row in cases),
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
    sdk = args.sdk_root or Path(__file__).resolve().parents[3]
    report = qualify(sdk, args.output_dir, args.jobs)
    print(json.dumps({'status': report['status'], 'public_surface_executions': report['executions'],
                      'public_surface_rejections': report['rejections']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
