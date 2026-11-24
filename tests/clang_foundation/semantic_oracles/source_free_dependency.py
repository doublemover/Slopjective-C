"""Run the reviewed ownership oracle through a relocated, locked re-export.

Run with PYTHONPATH=. from the checkout. The facade can be the SDK's objc3c or
a separately rebuilt binary compatible with its driver and runtime.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
from typing import NamedTuple

from scripts.objc3c_toolchain.binary_packages import digest
from scripts.objc3c_toolchain.linux_host import native_target, require_elf
from scripts.objc3c_toolchain.llvm_ir_execution import Execution
from scripts.objc3c_toolchain.package import verify_sdk
from scripts.objc3c_toolchain.sdk_example import environment
from scripts.objc3c_toolchain.semantic_model import generate, mismatch
from scripts.objc3c_toolchain.semantic_oracles import EXPORTS, source_bundle


PACKAGE = 'semantic-oracle-v1'
PROVIDER = 'oracle.Provider'
ENTRY = 'oracle.Entry'
SEED = 7


class Provider(NamedTuple):
    interface: Path
    object_file: Path
    identity: dict
    sources: Path


def check(condition, message):
    if not condition:
        raise ValueError(message)


def sidecar(interface):
    return interface.with_suffix('.objc3i.json')


def lock(identity):
    return ('--import-module', identity['canonical_name'], identity['semantic_fingerprint'], PACKAGE)


def copy_interface(interface, destination):
    for path in (interface, sidecar(interface)):
        shutil.copyfile(path, destination / path.name)


def qualify(sdk: Path, facade: Path, output: Path):
    sdk, facade, output = sdk.resolve(strict=True), facade.resolve(strict=True), output.resolve()
    check(not (output.is_relative_to(sdk) or sdk.is_relative_to(output)),
          'qualification output must be disjoint from the SDK')
    check(not output.exists(), 'qualification requires a fresh output directory')
    require_elf(facade)
    target = native_target()
    manifest = verify_sdk(sdk)
    check(manifest['target'] == target, 'SDK target differs from the native host')
    clang, services, runtime = (sdk / name for name in
        ('bin/clang++', 'lib/libobjc3_runtime_services.a', 'lib/libobjc.so'))
    report = {'schema': 'objc3c.semantic-source-free-dependency.v1', 'status': 'running',
              'target': target, 'seed': SEED, 'sdk_seal': manifest['seal_sha256'],
              'inputs': {str(path): digest(path) for path in (facade, clang, services, runtime)},
              'ownership_change': {'declaration': 'oracle_make', 'before': 'ns_returns_retained',
                                   'after': 'ns_returns_not_retained'}, 'cases': []}
    execution = Execution(output, environment(sdk), report)
    execution.save()
    program = generate(SEED)
    try:
        for optimization in ('O0', 'O2'):
            lane = output / optimization
            lane.mkdir()
            flags = ['--no-default-config', '--target=' + target, '-std=c++20', '-' + optimization,
                     '-fexceptions', '-fobjc-exceptions', '-fobjc-arc-exceptions', '-fblocks',
                     '-isystem', sdk / 'include']

            def compile_source(name, source, options, *, outputs=(), expected=0):
                cache = lane / (name + '-module-cache')
                check(not cache.exists(), 'module cache must be fresh')
                return execution.run(optimization + '-' + name,
                    [facade, source, '--objc3', '--objcxx', '--arc', '--sdk-root', sdk, *options,
                     '--', *flags, '-fmodules', '-fmodules-cache-path=' + str(cache)],
                    cwd=source.parent, outputs=outputs, stage='frontend-interface', expected=expected)

            providers = {}
            for name, bias in (('v1', 0), ('private-v2', 1), ('ownership-v2', 1)):
                work = lane / name
                sources = work / 'source'
                source_bundle(program, sources, transport='interface', bias=bias,
                              optimization=optimization, origin={'seed': SEED})
                if name == 'ownership-v2':
                    header = sources / 'oracle.h'
                    declarations = header.read_text()
                    check(declarations.count('ns_returns_retained') == 1, 'expected one ownership contract')
                    header.write_text(declarations.replace('ns_returns_retained', 'ns_returns_not_retained'))
                interface, obj = work / (PROVIDER + '.objc3i'), work / 'provider.o'
                compile_source(name + '-provider', sources / 'provider.mm',
                    ['--emit-object', obj, '--emit-interface', interface, '--module', PROVIDER,
                     '--package', PACKAGE, '--module-owned-names', '--module-source-root', sources,
                     *(part for name_to_export in EXPORTS for part in ('--export-declaration', name_to_export))],
                    outputs=(obj, interface, sidecar(interface)))
                retained = work / 'retained-source'
                sources.rename(retained)
                providers[name] = Provider(interface, obj, json.loads(sidecar(interface).read_text())['module'], retained)

            v1, private, changed = (providers[name] for name in ('v1', 'private-v2', 'ownership-v2'))
            check(v1.identity == private.identity, 'private body edit changed public identity')
            check(private.identity['semantic_fingerprint'] != changed.identity['semantic_fingerprint'],
                  'public ownership edit preserved identity')
            expected = {name: json.loads((providers[name].sources / 'program.json').read_text())['expected']
                        for name in ('v1', 'private-v2')}
            check(mismatch(expected['v1'], expected['private-v2']) is not None,
                  'generated case must observe the private provider change')
            directive = f'@import {PROVIDER};\n'
            consumer = (v1.sources / 'consumer.mm').read_text()
            check(consumer.startswith(directive), 'expected the generated source-directed import')
            consumer = f'@import {ENTRY};\n' + consumer.removeprefix(directive)

            def reexport(name):
                provider = providers[name]
                work = lane / name / 'entry'
                work.mkdir()
                copy_interface(provider.interface, work)
                source = work / 'entry.mm'
                source.write_text(f'@import {PROVIDER};\nint oracle_entry_version(void) {{ return 1; }}\n')
                entry = work / (ENTRY + '.objc3i')
                compile_source(name + '-entry', source,
                    ['--source-imports', '--module-search-path', work, *lock(provider.identity), '--reexport-import',
                     '--emit-interface', entry, '--module', ENTRY, '--package', PACKAGE, '--module-owned-names'],
                    outputs=(entry, sidecar(entry)))
                return entry, json.loads(sidecar(entry).read_text())['module']

            original, old_entry = reexport('v1')
            refreshed, new_entry = reexport('ownership-v2')
            check(old_entry != new_entry, 'changed dependency did not propagate into the re-export identity')
            for variant, entry, entry_id, outcome in (
                    ('v1', original, old_entry, 'execute'), ('private-v2', original, old_entry, 'execute'),
                    ('ownership-v2', original, old_entry, 'reject'), ('ownership-v2', refreshed, new_entry, 'compile')):
                name = variant + '-' + outcome
                provider = providers[variant]
                work = lane / name
                modules = work / 'relocated-interfaces'
                modules.mkdir(parents=True)
                copy_interface(provider.interface, modules)
                copy_interface(entry, modules)
                check(not (provider.sources.parent / 'source').exists(), 'provider source still exists at its original path')
                source = work / 'consumer.mm'
                source.write_text(consumer)
                client, record = work / 'consumer.o', work / 'dependencies.json'
                rejected = outcome == 'reject'
                log = compile_source(name, source,
                    ['--source-imports', '--module-search-path', modules, *lock(entry_id), '--emit-object', client,
                     '--diagnostics-json', '--emit-dependency-record', record],
                    outputs=() if rejected else (client, record), expected=int(rejected))
                if rejected:
                    diagnostic = next(json.loads(line) for line in log.splitlines() if line.startswith('{'))
                    check(any(row['code'] == 'O3IFC134' and row['subject'] == PROVIDER and row['stage'] == 'import'
                              for row in diagnostic['interface_diagnostics']), 'missing stale dependency diagnostic')
                    check(not (client.exists() or record.exists()), 'stale dependency published consumer output')
                else:
                    dependencies = json.loads(record.read_text())
                    check({row['name']: row['fingerprint'] for row in dependencies['modules']} == {
                        ENTRY: entry_id['semantic_fingerprint'], PROVIDER: provider.identity['semantic_fingerprint']},
                        'consumer used different locked modules')
                    check(dependencies['edges'] == [{'from': ENTRY, 'to': PROVIDER, 'public': True, 'reexported': True}],
                          'consumer did not use the public re-export')
                if outcome == 'execute':
                    binary = work / 'consumer'
                    execution.run(optimization + '-' + name + '-link',
                        [clang, '--no-default-config', '-fuse-ld=lld', provider.object_file, client, services, runtime,
                         '-pthread', '-Wl,-rpath,' + str(runtime.parent), '-o', binary], outputs=(binary,))
                    actual = json.loads(execution.run(optimization + '-' + name + '-run', [binary],
                                                      stage='runtime-oracle'))
                    classification = mismatch(expected[variant], actual)
                    check(classification is None, 'independent oracle disagreement: ' + str(classification))
                report['cases'].append({'name': name, 'optimization': optimization, 'provider': provider.identity,
                                        'entry': entry_id, 'status': 'PASS'})
                execution.save()
        report['status'] = 'PASS'
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        execution.save()
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--facade', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = qualify(args.sdk_root, args.facade, args.output)
    print(json.dumps({'status': result['status'], 'cases': len(result['cases'])}))
