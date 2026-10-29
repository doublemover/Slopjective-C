"""Qualify installed package graphs, transitive visibility and content reuse."""
from __future__ import annotations

import argparse
from contextvars import copy_context
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


def qualify_case(sdk: Path, output: Path, language: str, standard: str, ownership: str, optimization: str) -> tuple[list, list]:
    output.mkdir(parents=True, exist_ok=True)
    records, negatives = [], []
    with tempfile.TemporaryDirectory(prefix="objc3 packages 'outside checkout ") as temporary:
        root = Path(temporary).resolve()
        if any((parent / '.git').exists() for parent in (root, *root.parents)):
            raise ValueError('package qualification must run outside a checkout')
        sources = root / 'source graph'
        shutil.copytree(sdk / 'share/objc3c/examples/packages', sources)
        original = {str(path.relative_to(sources)): path.read_bytes()
                    for path in sources.rglob('*') if path.is_file()}
        suffix = {'objective-c': '.m', 'objective-c++': '.mm', 'c++': '.cpp'}[language]
        case_prefix = language + '-' + standard + '-' + ownership + '-' + optimization
        for manifest in sources.glob('*/package.json'):
            data = json.loads(manifest.read_text())
            original_source = manifest.parent / data['source']
            source = original_source.with_suffix(suffix)
            if source != original_source: original_source.rename(source)
            data.update(ownership=ownership, provider_ownership=ownership, language=language, standard=standard, source=source.name)
            manifest.write_text(json.dumps(data))
        workspace = sources / 'workspace.json'
        data = json.loads(workspace.read_text())
        original_source = sources / data['application']['root'] / data['application']['source']
        source = original_source.with_suffix(suffix)
        if source != original_source: original_source.rename(source)
        data['application'].update(ownership=ownership, language=language, standard=standard, source=source.name)
        workspace.write_text(json.dumps(data))
        build = sources / 'build' / case_prefix
        all_nodes = {'fixture.Leaf', 'fixture.Left', 'fixture.Right', 'fixture.Top', '@application'}

        def run(label, expected):
            log = output / f'{case_prefix}-{label}.log'
            with log.open('w') as stream:
                completed = subprocess.run([str(sdk / 'bin/objc3c-build'), '--workspace', str(sources / 'workspace.json'),
                    '--output-dir', str(build), '--optimization', optimization],
                    cwd=root, stdout=stream, stderr=subprocess.STDOUT, check=False, timeout=180)
            if completed.returncode:
                for failure_log in build.rglob('compile.log'):
                    destination = output / (case_prefix + '-' + label + '-' + failure_log.parent.name + '.failure.log')
                    shutil.copyfile(failure_log, destination)
                raise ValueError(f'installed graph build failed; see {log}')
            report = json.loads((build / 'build-report.json').read_text())
            actual = {name for name, row in report['nodes'].items() if row['reused']}
            if actual != set(expected):
                raise ValueError(f'{optimization} {label}: reused {actual}; expected {set(expected)}')
            records.append({'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization, 'case': label, 'reused': sorted(actual),
                'owned_transitive_record': language == 'objective-c++' and ownership == 'arc',
                'sdk_seal': report['sdk_seal'], 'installation': report['installation']['content_sha256']})
            shutil.copyfile(build / 'build-report.json', output / f'{case_prefix}-{label}.json')
            return report

        cold = run('cold', ())
        warm = run('warm', all_nodes)
        installation = cold['installation']
        expected_mode = {'language': language, 'standard': standard, 'ownership': ownership}
        if (installation['schema'] != 'objc3c.package-installation.v4' or
                installation['application_mode'] != expected_mode or
                any(any(package[key] != value for key, value in expected_mode.items())
                    for package in installation['packages'])):
            raise ValueError('package installation did not preserve its explicit language mode')
        if cold['installation'] != warm['installation']:
            raise ValueError('unchanged builds did not reproduce the package installation inventory')
        leaf = sources / ('leaf/module' + suffix)
        stamp = leaf.stat()
        leaf.write_bytes(original['leaf/module.m'].replace(b'return 40;', b'volatile int value = 40; return value;'))
        os.utime(leaf, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
        body = run('leaf-body-preserved-mtime', all_nodes - {'fixture.Leaf'})
        if cold['modules'] != body['modules']:
            raise ValueError('a private implementation edit changed module identities')
        header = sources / 'right/private.h'
        stamp = header.stat()
        header.write_bytes(original['right/private.h'] + b'\n// private implementation edit\n')
        os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
        run('private-header-preserved-mtime', all_nodes - {'fixture.Right'})
        (build / 'modules/fixture.Left/module.o').write_bytes(b'damaged object')
        run('damaged-object', all_nodes - {'fixture.Left'})
        interface = build / 'modules/fixture.Leaf/fixture.Leaf.objc3i'
        interface.write_bytes(interface.read_bytes() + b'\n// damaged interface\n')
        run('damaged-interface', all_nodes - {'fixture.Leaf'})
        leaf.write_bytes(leaf.read_bytes() + b'\nint added_public_api(void) { return 0; }\n')
        changed = run('transitive-public-api', ())
        if changed['modules']['fixture.Leaf'] == cold['modules']['fixture.Leaf']:
            raise ValueError('a new public declaration did not change the module identity')
        # A manifest lock is resolved before executing any compiler command.
        manifest = sources / 'top/package.json'
        content = manifest.read_bytes()
        prior_report = (build / 'build-report.json').read_bytes()
        data = json.loads(content)
        data['dependencies'][0]['package'] = 'wrong-identity'
        manifest.write_text(json.dumps(data))
        command = [str(sdk / 'bin/objc3c-build'), '--workspace', str(sources / 'workspace.json'),
                   '--output-dir', str(build), '--optimization', optimization]
        rejected = subprocess.run(command, cwd=root, capture_output=True, text=True, timeout=180)
        if (rejected.returncode != 1 or 'identity lock' not in rejected.stderr or
                (build / 'build-report.json').read_bytes() != prior_report):
            raise ValueError('wrong package lock did not reject before changing a prior build record')
        manifest.write_bytes(content)
        negatives.append({'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization, 'case': 'package-lock', 'prior_record_preserved': True})
        # Top re-exports Left -> Leaf, while its Right edge is private.
        # The cold application already used Leaf's transitive public type.
        app = sources / ('app/main' + suffix)
        app.write_bytes(original['app/main.m'].replace(b'@fixture.Top.answer()', b'@fixture.Right.right()') if language != 'c++' else original['app/main.m'].replace(b'answer() == 42', b'right() == 42'))
        obj = build / 'application/module.o'
        before = hashlib.sha256(obj.read_bytes()).hexdigest()
        rejected = subprocess.run(command, cwd=root, capture_output=True, text=True, timeout=180)
        log = build / 'application/compile.log'
        outcome = json.loads((build / 'application/compile-result.json').read_text())
        if (rejected.returncode != 1 or outcome['exit_code'] != 1 or ('fixture.Right' if language != 'c++' else 'right') not in log.read_text() or
                hashlib.sha256(obj.read_bytes()).hexdigest() != before or
                (build / 'build-report.json').exists()):
            raise ValueError('private transitive visibility did not reject with the object preserved')
        shutil.copyfile(log, output / f'{case_prefix}-private-visibility.log')
        negatives.append({'language': language, 'standard': standard, 'ownership': ownership, 'optimization': optimization, 'case': 'private-transitive-visibility',
                          'prior_object_preserved': True, 'failed_build_record_removed': True,
                          'exit_code': rejected.returncode, 'compiler_exit_code': outcome['exit_code'],
                          'expected_exit_code': 1})
    return records, negatives


def qualify(sdk: Path, output: Path, jobs: int = 1) -> dict:
    from concurrent.futures import ThreadPoolExecutor

    if jobs not in range(1, 5):
        raise ValueError('qualification jobs must be between 1 and 4')
    sdk, output = sdk.resolve(), output.resolve()
    if output == sdk or sdk in output.parents or output in sdk.parents:
        raise ValueError('qualification output must be disjoint from the SDK')
    output.mkdir(parents=True, exist_ok=True)
    receipt_path = output / 'qualification.json'
    receipt_path.unlink(missing_ok=True)
    modes = [('objective-c', 'c17', ownership) for ownership in ('arc', 'mrc')]
    modes += [('objective-c++', standard, ownership)
              for standard in ('c++20', 'c++23') for ownership in ('arc', 'mrc')]
    modes += [('c++', standard, 'none') for standard in ('c++20', 'c++23')]
    cases = [(*mode, optimization) for mode in modes for optimization in ('O0', 'O2')]
    records, negatives, failures = [], [], []
    # Each case has its own temporary source tree and log prefix. Joining the
    # executor preserves evidence from other cases when any one case fails.
    with ThreadPoolExecutor(max_workers=jobs) as executor:
        futures = [executor.submit(copy_context().run, qualify_case, sdk, output, *case) for case in cases]
        for case, future in zip(cases, futures):
            try:
                built, rejected = future.result()
                records.extend(built)
                negatives.extend(rejected)
            except Exception as error:
                failures.append({'mode': case, 'error': str(error)})
    if failures:
        receipt_path.write_text(json.dumps({'schema': 'objc3c.source-graph-qualification.v4',
            'status': 'FAIL', 'failures': failures, 'builds': records, 'rejections': negatives}, indent=2) + '\n')
        raise ValueError(f'installed graph qualification failed; see {receipt_path}')
    if not (len(records) == 112 and len(negatives) == 32):
        raise ValueError('qualify_source_graph: required semantic check failed: len(records) == 112 and len(negatives) == 32')
    result = {'schema': 'objc3c.source-graph-qualification.v4', 'status': 'PASS',
              'outside_checkout': True, 'paths_with_spaces_and_quotes': True,
              'builds': records, 'rejections': negatives}
    (output / 'qualification.json').write_text(json.dumps(result, indent=2) + '\n')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path)
    parser.add_argument('--jobs', type=int, choices=range(1, 5), default=2)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    sdk = (args.sdk_root or Path(__file__).resolve().parents[3]).resolve()
    report = qualify(sdk, args.output_dir.resolve(), args.jobs)
    print(json.dumps({'status': report['status'], 'builds': len(report['builds']),
                      'rejections': len(report['rejections'])}))


if __name__ == '__main__':
    main()
