"""Match generated ownership behavior and lock admission across a native re-export."""
from __future__ import annotations

import json
from pathlib import Path
import shutil
from typing import NamedTuple

from .semantic_model import mismatch


PACKAGE = 'semantic-oracle-v1'
PROVIDER = 'oracle.Provider'
ENTRY = 'oracle.Entry'


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


def exercise_reexport(output: Path, run, *, facade, clang, services, runtime, flags,
                      program, origin, prepare_bundle, exports, optimization='O2', frontend_options=()):
    """Compile real variants and reuse one source-free consumer under exact locks."""
    output.mkdir()
    flags = [*flags, '-std=c++20', '-' + optimization]
    case_prefix = 'source-free-reexport-' + optimization + '-'
    cases = []

    def compile_source(name, source, options, *, outputs=(), expected=0):
        cache = output / (name + '-module-cache')
        check(not cache.exists(), 'module cache must be fresh')
        return run(case_prefix + name,
            [facade, source, '--objc3', '--objcxx', '--arc', *frontend_options, *options,
             '--', *flags, '-fmodules', '-fmodules-cache-path=' + str(cache)],
            cwd=source.parent, outputs=outputs, stage='frontend-interface', expected=expected)

    providers = {}
    for name, bias in (('v1', 0), ('private-v2', 1), ('ownership-v2', 1)):
        work = output / name
        sources = work / 'source'
        prepare_bundle(program, sources, transport='interface', bias=bias,
                      optimization=optimization, origin=origin)
        if name == 'ownership-v2':
            header = sources / 'oracle.h'
            declarations = header.read_text()
            check(declarations.count('ns_returns_retained') == 1, 'expected one ownership contract')
            header.write_text(declarations.replace('ns_returns_retained', 'ns_returns_not_retained'))
        interface, obj = work / (PROVIDER + '.objc3i'), work / 'provider.o'
        compile_source(name + '-provider', sources / 'provider.mm',
            ['--emit-object', obj, '--emit-interface', interface, '--module', PROVIDER,
             '--package', PACKAGE, '--module-owned-names', '--module-source-root', sources,
             *(part for name_to_export in exports for part in ('--export-declaration', name_to_export))],
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
        work = output / name / 'entry'
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
    check(old_entry['semantic_fingerprint'] != new_entry['semantic_fingerprint'],
          'changed dependency did not propagate into the re-export identity')
    for variant, entry, entry_id, outcome in (
            ('v1', original, old_entry, 'execute'), ('private-v2', original, old_entry, 'execute'),
            ('ownership-v2', original, old_entry, 'reject'), ('ownership-v2', refreshed, new_entry, 'compile')):
        name = variant + '-' + outcome
        provider = providers[variant]
        work = output / name
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
            diagnostic = next((json.loads(line) for line in log.splitlines() if line.startswith('{')), {})
            check(any(row['code'] == 'O3IFC134' and row['subject'] == PROVIDER and row['stage'] == 'import'
                      for row in diagnostic.get('interface_diagnostics', [])), 'missing stale dependency diagnostic')
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
            run(case_prefix + name + '-link',
                [clang, '--no-default-config', '-fuse-ld=lld', provider.object_file, client, services, runtime,
                 '-pthread', '-Wl,-rpath,' + str(runtime.parent), '-o', binary], outputs=(binary,))
            actual = json.loads(run(case_prefix + name + '-run', [binary], stage='runtime-oracle'))
            classification = mismatch(expected[variant], actual)
            check(classification is None, 'independent oracle disagreement: ' + str(classification))
        cases.append({'name': name, 'optimization': optimization, 'provider': provider.identity,
                      'entry': entry_id, 'status': 'PASS'})
    return cases
