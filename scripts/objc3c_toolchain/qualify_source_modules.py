"""Qualify installed source modules, invalidation and rejected identity locks."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


def qualify(sdk: Path, output: Path) -> dict:
    output.mkdir(parents=True, exist_ok=True)
    example = sdk / 'share/objc3c/examples/managed'
    records = []
    negative = []
    with tempfile.TemporaryDirectory(prefix='objc3-source-modules-') as temporary:
        client = Path(temporary)
        if any((parent / '.git').exists() for parent in (client, *client.parents)):
            raise ValueError('installed source qualification must run outside a checkout')
        sources = client / "source with spaces 'and quotes"
        shutil.copytree(example / 'src', sources)
        original = {name: (sources / name).read_bytes()
                    for name in ('provider.m', 'managed-value-support.h', 'main.m')}
        for optimization in ('O0', 'O2'):
            for name, content in original.items():
                (sources / name).write_bytes(content)
            build = client / optimization

            def run(name, expected):
                log_path = output / f'{optimization}-{name}.log'
                with log_path.open('w') as log:
                    subprocess.run([os.sys.executable, '-B', str(example / 'build.py'),
                        '--source-dir', str(sources), '--output-dir', str(build),
                        '--optimization', optimization], cwd=client, stdout=log,
                        stderr=subprocess.STDOUT, check=True, timeout=180)
                report = json.loads((build / 'build-report.json').read_text())
                actual = {stage for stage, row in report['stages'].items() if row['reused']}
                if actual != set(expected):
                    raise ValueError(f'{optimization} {name}: reused {actual}, expected {set(expected)}')
                records.append({'optimization': optimization, 'case': name,
                    'reused_stages': sorted(actual), 'build_seconds': report['build_seconds'],
                    'sdk_admission_seconds': report['sdk_admission_seconds'],
                    'sdk_final_verification_seconds': report['sdk_final_verification_seconds'],
                    'elapsed_seconds': report['elapsed_seconds'], 'sdk_seal': report['sdk_seal'],
                    'fingerprint': report['module_identity']['semantic_fingerprint']})
                shutil.copyfile(build / 'build-report.json', output / f'{optimization}-{name}.json')
                return report

            all_stages = ('provider', 'arc-interface', 'consumer')
            cold = run('cold', ())
            run('warm', all_stages)
            dependency = (build / 'consumer-dependencies.json').read_bytes()
            provider = sources / 'provider.m'
            stamp = provider.stat()
            provider.write_bytes(original['provider.m'].replace(b'return 42;', b'volatile int result = 42; return result;', 1))
            os.utime(provider, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
            body = run('provider-body-preserved-mtime', ('arc-interface', 'consumer'))
            if body['module_identity'] != cold['module_identity']:
                raise ValueError('a provider body edit changed its semantic interface identity')
            header = sources / 'managed-value-support.h'
            stamp = header.stat()
            header.write_bytes(original[header.name] + b'\n// Private implementation edit.\n')
            os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
            run('private-header-preserved-mtime', ('arc-interface', 'consumer'))
            # Reconstruct the same dependency record from a real recompilation.
            (build / 'consumer.o').unlink()
            run('missing-consumer-output', ('provider', 'arc-interface'))
            if (build / 'consumer-dependencies.json').read_bytes() != dependency:
                raise ValueError('unchanged inputs did not reproduce the dependency record')
            interface = build / 'modules/NativePackage.objc3i'
            stamp = interface.stat()
            content = interface.read_bytes()
            interface.write_bytes(content.replace(b'Answer', b'Broken', 1))
            os.utime(interface, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
            run('damaged-interface-preserved-mtime', ('provider', 'consumer'))
            # A new exported declaration must invalidate the identity and consumer.
            provider.write_bytes(provider.read_bytes() + b'\nAnswer extra_answer(void) { return 42; }\n')
            changed = run('provider-api', ())
            if changed['module_identity']['semantic_fingerprint'] == cold['module_identity']['semantic_fingerprint']:
                raise ValueError('new public API did not change the semantic fingerprint')
            dependency_record = json.loads((build / 'consumer-dependencies.json').read_text())
            shutil.copyfile(build / 'consumer-dependencies.json', output / f'{optimization}-dependencies.json')
            identity = changed['module_identity']
            for case, fingerprint, package, arc, code in (
                ('fingerprint', '0' * 64, identity['package_identity'], True, 'O3IFC134'),
                ('package', identity['semantic_fingerprint'], 'wrong-package', True, 'O3IFC134'),
                # CLI locks fill the remaining identity fields from the
                # consumer mode, so ARC/MRC disagreement fails exact lookup.
                ('ownership', identity['semantic_fingerprint'], identity['package_identity'], False, 'O3IFC134')):
                obj = client / (optimization + '-' + case + '.o')
                deps = obj.with_suffix('.json')
                command = [str(sdk / 'bin/objc3c'), str(sources / 'main.m'), '--objc3',
                    '--target', identity['target_triple'], '--runtime', 'gnustep-2.0',
                    '--source-imports', '--module-search-path', str(build / 'modules'),
                    '--import-module', identity['canonical_name'], fingerprint, package,
                    '--emit-object', str(obj), '--emit-dependency-record', str(deps)]
                if arc:
                    command.append('--arc')
                command += ['--', '--no-default-config', '-fblocks', '-' + optimization]
                result = subprocess.run(command, cwd=client, capture_output=True, text=True, timeout=180)
                (output / f'{optimization}-reject-{case}.log').write_text(result.stdout + result.stderr)
                if result.returncode == 0 or code not in result.stderr or obj.exists() or deps.exists():
                    raise ValueError(f'{optimization} {case}: invalid import did not fail atomically with {code}')
                negative.append({'optimization': optimization, 'case': case,
                                 'exit_code': result.returncode, 'diagnostic': code, 'outputs_absent': True})
            if dependency_record['modules'][0]['fingerprint'] != identity['semantic_fingerprint']:
                raise ValueError('dependency lock did not update after the API edit')
    result = {'schema': 'objc3c.installed-source-modules.v1', 'status': 'PASS',
              'sdk_seal': records[0]['sdk_seal'], 'outside_checkout': True,
              'builds': records, 'rejected_imports': negative,
              'source_builder_sha256': hashlib.sha256((example / 'build.py').read_bytes()).hexdigest(),
              'limits': 'Example module graph, not a general package manager. Build timings exclude full SDK admission and final verification; elapsed timings include both. The native linker runs on every invocation.'}
    (output / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', required=True, type=Path)
    parser.add_argument('--sdk', type=Path)
    args = parser.parse_args()
    sdk = (args.sdk or Path(__file__).resolve().parents[4]).resolve()
    tick = time.monotonic()
    result = qualify(sdk, args.output_dir.resolve())
    print(json.dumps({'status': result['status'], 'elapsed_seconds': round(time.monotonic() - tick, 6),
                      'summary': str(args.output_dir.resolve() / 'summary.json')}))


if __name__ == '__main__':
    main()
