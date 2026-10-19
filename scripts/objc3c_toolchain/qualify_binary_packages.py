"""Qualify immutable publication, relocation and source-free package consumers."""
from __future__ import annotations

import argparse
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


def qualify_case(sdk: Path, output: Path, language: str, standard: str, ownership: str, optimization: str) -> dict:
    label = '-'.join((language, standard, ownership, optimization))
    with tempfile.TemporaryDirectory(prefix="objc3 binary 'outside checkout ") as temporary:
        root = Path(temporary)
        sources = root / 'sources'
        shutil.copytree(sdk / 'share/objc3c/examples/packages', sources)
        suffix = {'objective-c': '.m', 'objective-c++': '.mm', 'c++': '.cpp'}[language]
        for manifest in sources.glob('*/package.json'):
            data = json.loads(manifest.read_text())
            old = manifest.parent / data['source']
            new = old.with_suffix(suffix)
            if old != new:
                old.rename(new)
            data.update(language=language, standard=standard, ownership=ownership, source=new.name)
            manifest.write_text(json.dumps(data))
        workspace = sources / 'workspace.json'
        config = json.loads(workspace.read_text())
        app = config['application']
        old = sources / app['root'] / app['source']
        new = old.with_suffix(suffix)
        if old != new:
            old.rename(new)
        app.update(language=language, standard=standard, ownership=ownership, source=new.name)
        workspace.write_text(json.dumps(config))
        built = root / 'built'
        build_graph(sdk, workspace, built, optimization)
        published = packages.publish(sdk, built, root / 'publication')
        # Re-publication is reproducible and reuses the verified immutable entry.
        if packages.publish(sdk, built, root / 'publication') != published:
            raise ValueError('unchanged publication did not reproduce its content identity')
        package = Path(published['package'])
        installed = packages.install(package, published['content_sha256'], root / 'installed')
        if packages.install(package, published['content_sha256'], root / 'installed') != installed:
            raise ValueError('unchanged installation was not idempotent')
        relocated = root / 'relocated packages'
        (root / 'installed').rename(relocated)
        package = relocated / published['content_sha256']
        client = root / 'client'
        client.mkdir()
        shutil.copyfile(new, client / new.name)
        application = client / 'application.json'
        application.write_text(json.dumps({'schema': packages.CONSUMER, **{key: app[key] for key in ('source', *packages.MODE, 'dependencies')}}))
        shutil.rmtree(sources)
        shutil.rmtree(built)
        shutil.rmtree(root / 'publication')
        report = packages.consume(sdk, package, published['content_sha256'], application, root / 'consumer')
        if report['provider_compilations'] or len(report['commands']) != 2 or len(report['modules']) != 4:
            raise ValueError('binary consumer rebuilt a provider or lost its dependency closure')
        packages.write_json(output / (label + '-consumer.json'), report)
        negatives = []
        for case in ('wrong-content', 'wrong-sdk', 'wrong-mode', 'wrong-package', 'private-visibility', 'damaged-object', 'damaged-interface', 'damaged-sidecar'):
            expected = published['content_sha256']
            original = application.read_bytes()
            original_source = (client / new.name).read_bytes()
            damaged, payload = None, None
            if case == 'wrong-content':
                expected = '0' * 64
            elif case == 'wrong-sdk':
                damaged = package / 'package.json'
                payload = damaged.read_bytes()
                data = json.loads(payload)
                data['sdk_seal'] = '0' * 64
                expected = packages.content_hash(data)
                damaged.chmod(0o644)
                damaged.write_text(json.dumps(data))
            elif case in ('wrong-mode', 'wrong-package'):
                data = json.loads(original)
                if case == 'wrong-mode':
                    data['ownership'] = 'mrc' if ownership == 'arc' else 'arc'
                else:
                    data['dependencies'][0]['package'] = 'wrong-package-lock'
                application.write_text(json.dumps(data))
            elif case == 'private-visibility':
                source = client / new.name
                source.write_bytes(original_source.replace(b'@fixture.Top.answer()', b'@fixture.Right.right()') if language != 'c++' else original_source.replace(b'answer() == 42', b'right() == 42'))
            else:
                extension = {'damaged-object': 'module.o', 'damaged-interface': 'fixture.Leaf.objc3i', 'damaged-sidecar': 'fixture.Leaf.objc3i.json'}[case]
                damaged = package / 'modules/fixture.Leaf' / extension
                payload = damaged.read_bytes()
                damaged.chmod(0o644)
                damaged.write_bytes(b'damaged artifact')
            rejected = root / 'consumer' if case == 'private-visibility' else root / ('rejected-' + case)
            prior_object = packages.digest(rejected / 'application.o') if case == 'private-visibility' else None
            try:
                packages.consume(sdk, package, expected, application, rejected)
            except (ValueError, OSError, packages.subprocess.SubprocessError) as error:
                reasons = {'wrong-content': ('content identity differs',),
                           'wrong-sdk': ('exact producing SDK',),
                           'wrong-mode': ('identity or mode lock', 'ownership must'),
                           'wrong-package': ('identity or mode lock',)}
                if case != 'private-visibility' and not any(
                        reason in str(error) for reason in reasons.get(case, ('artifact hash differs',))):
                    raise ValueError('binary consumer rejected for an unexpected reason: ' + case) from error
                if (rejected / 'consumer-report.json').exists():
                    raise ValueError('a rejected consumer retained a success report') from error
                if case != 'private-visibility' and rejected.exists():
                    raise ValueError('invalid package lock reached compilation') from error
                compile_log = rejected / 'command-0.log'
                logs = [compile_log] if compile_log.exists() else []
                if case == 'private-visibility' and (not logs or ('right' not in compile_log.read_text())):
                    raise ValueError('private module rejection lacks the expected diagnostic') from error
                if prior_object and packages.digest(rejected / 'application.o') != prior_object:
                    raise ValueError('rejected consumer replaced its prior object') from error
                for log in logs:
                    shutil.copyfile(log, output / (label + '-' + case + '-' + log.name))
                negatives.append(case)
            else:
                raise ValueError('invalid binary consumer was accepted: ' + case)
            finally:
                application.write_bytes(original)
                (client / new.name).write_bytes(original_source)
                if damaged:
                    damaged.write_bytes(payload)
                    damaged.chmod(0o444)
        return {'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization,
                'content_sha256': published['content_sha256'], 'sdk_seal': report['sdk_seal'],
                'modules': report['modules'], 'provider_compilations': 0, 'relocated': True,
                'producer_sources_removed': True, 'failed_consumer_record_removed': True,
                'rejected_object_preserved': True, 'rejections': negatives}


def qualify(sdk: Path, output: Path) -> dict:
    sdk, output = sdk.resolve(strict=True), output.resolve()
    packages.disjoint(output, sdk)
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    receipt.unlink(missing_ok=True)
    modes = [('objective-c', 'c17', ownership) for ownership in ('arc', 'mrc')]
    modes += [('objective-c++', standard, ownership) for standard in ('c++20', 'c++23') for ownership in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    cases = []
    try:
        for mode in modes:
            for optimization in ('O0', 'O2'):
                cases.append(qualify_case(sdk, output, *mode, optimization))
    except Exception as error:
        packages.write_json(receipt, {'schema': 'objc3c.binary-package-qualification.v1', 'status': 'FAIL', 'cases': cases, 'error': str(error)})
        raise
    report = {'schema': 'objc3c.binary-package-qualification.v1', 'status': 'PASS',
              'outside_checkout': True, 'paths_with_spaces_and_quotes': True, 'executions': len(cases),
              'rejections': sum(len(row['rejections']) for row in cases), 'cases': cases}
    packages.write_json(receipt, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    sdk = args.sdk_root or Path(__file__).resolve().parents[3]
    report = qualify(sdk, args.output_dir)
    print(json.dumps({'status': report['status'], 'binary_package_executions': report['executions'], 'binary_package_rejections': report['rejections']}))


if __name__ == '__main__':
    main()
