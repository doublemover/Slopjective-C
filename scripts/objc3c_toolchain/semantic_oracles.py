"""Compare generated native ObjC3 programs with an independent semantic model."""
from __future__ import annotations

import argparse
from dataclasses import asdict
import hashlib
import json
from pathlib import Path
import re
import shutil

from .binary_packages import digest, write_json
from .compiler_instrumentation import loaded_graph
from .llvm_ir_execution import Execution, StageFailure
from .llvm_tools import identity, selected, verify_receipt
from .sdk_example import dependencies
from .semantic_model import VERSION, Operation, candidates, coverage, generate, interpret, mismatch, validate
from .semantic_sources import HEADER, consumer, provider

EXPORTS = ('OracleResult', 'oracle_make', 'oracle_value', 'oracle_alive', 'oracle_destroyed', 'oracle_step')


def seed_bank(seeds, rotation=''):
    """Keep the reviewed bank and add reproducible bounded long-campaign seeds."""
    if rotation:
        if not re.fullmatch(r'[0-9a-f]{40}|[0-9a-f]{64}', rotation):
            raise ValueError('seed rotation requires the exact tested revision')
        seeds = [*seeds, *(int.from_bytes(hashlib.sha256((rotation+':'+str(i)).encode()).digest()[:4], 'big')
                            for i in range(3))]
    return tuple(dict.fromkeys(seeds))


def source_bundle(program, output: Path, *, transport='source', relation='', mutant='', bias=0,
                  optimization='O2', standard='c++20', sanitized=False, expected_mismatch=None):
    validate(program)
    if (transport not in ('source', 'pch', 'interface') or optimization not in ('O0', 'O2', 'thinlto') or
            standard not in ('c++20', 'c++23') or relation not in ('', 'rename', 'comments', 'pure-grouping') or
            mutant not in ('', 'tag', 'defer', 'destructor', 'provider', 'error') or
            type(bias) is not int or bias not in (0, 1) or type(sanitized) is not bool or
            expected_mismatch not in (None, 'value-or-lifetime', 'cleanup-order')):
        raise ValueError('unsupported semantic program configuration')
    output.mkdir(parents=True, exist_ok=False)
    write_json(output / 'program.json', {'model_version': VERSION, 'operations': [asdict(op) for op in program],
        'expected': interpret(program, bias=bias), 'transform': relation, 'mutant': mutant, 'provider_bias': bias,
        'transport': transport, 'optimization': optimization, 'standard': standard,
        'sanitized': sanitized, 'expected_mismatch': expected_mismatch,
        'requirements': {'target': 'x86_64-unknown-linux-gnu', 'ownership': 'arc', 'language': 'objective-c++'}})
    (output / 'oracle.h').write_text(HEADER)
    (output / 'provider.mm').write_text(provider(bias=bias, mutant=mutant if mutant in ('provider', 'error') else ''))
    (output / 'consumer.mm').write_text(consumer(program, transport=transport, relation=relation,
        mutant=mutant if mutant in ('tag', 'defer', 'destructor') else ''))


def shrink(program, predicate, expected_class: str):
    """Only a replayed mismatch of the same class can make a reduction."""
    current = list(program)
    while True:
        reduced = next((trial for trial in candidates(current) if predicate(trial) == expected_class), None)
        if reduced is None:
            return current
        current = reduced


class Campaign:
    def __init__(self, repo: Path, output: Path, facade: Path, services: Path, runtime: Path):
        self.repo, self.output = repo, output
        self.foundation, self.tools, environment = selected(repo)
        self.facade, self.services, self.runtime = facade.resolve(strict=True), services.resolve(strict=True), runtime.resolve(strict=True)
        self.report = {'schema': 'objc3c.generated-semantic-oracles.v1', 'status': 'running',
            'model_version': VERSION, 'source_build_id': self.foundation['source_build_id'],
            'build_config_digest': self.foundation['build_config_digest'],
            'permitted_environment': environment,
            'bounds': {'operations': 64, 'logical_allocations': 32, 'seeds': 8,
                       'process_timeout_seconds': 180,
                       'shared_executor_limits': self.foundation['host_environment']['resource_limits']},
            'profile': {'host': 'linux-x86_64', 'language': 'objective-c++', 'standards': ['c++20', 'c++23'],
                        'ownership': 'arc', 'target': 'x86_64-unknown-linux-gnu'},
            'cases': [], 'controls': [],
            'boundaries': ['ARC managed leaves and scalar carriers; MRC adaptation is not modeled.',
                'Exact logical survival/weak-zeroing/last destruction, not raw retain/release or copy counts.',
                'C++ scope and partial-construction cleanup order is explicitly sequenced.',
                'No move-from observations, signed overflow, unspecified effect order or floating-point equivalences.']}
        self.executor = Execution(output, environment, self.report, timeout=180)
        self.run = self.executor.run
        self.flags = ['--no-default-config', '--target=x86_64-unknown-linux-gnu', '-x', 'objective-c++',
            '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fobjc-arc', '-fobjc-arc-exceptions',
            '-fexceptions', '-fobjc-exceptions', '-fblocks', '-UNDEBUG',
            '-isystem', str(self.runtime.parent.parent / 'include')]
        stage = Path(self.foundation['paths']['stage'])
        self.inputs = {str(path): digest(path) for path in [Path(__file__),
            Path(__file__).with_name('semantic_model.py'), Path(__file__).with_name('semantic_sources.py'),
            self.facade, self.services, self.runtime, *self.tools.values(),
            *(stage / 'lib' / name for name in ('libLLVM.so', 'libclang-cpp.so'))]}
        for name in ('llvm_ir_execution.py', 'compiler_instrumentation.py', 'llvm_tools.py',
                     'binary_packages.py', 'sanitizer_policy.py', 'sdk_example.py'):
            path = Path(__file__).with_name(name)
            self.inputs[str(path)] = digest(path)
        self.report['inputs'] = self.inputs
        self.observations = {}
        libraries = [(stage / 'lib' / name).resolve(strict=True) for name in ('libLLVM.so', 'libclang-cpp.so')]
        self.report['compiler_graphs'] = {}
        for name, binary in (('clang', self.tools['clang++']), ('facade', self.facade)):
            graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(), libraries,
                required=tuple(libraries) if name == 'clang' else (libraries[0],),
                run=lambda suffix, argv, name=name: self.run(name+'-'+suffix, argv))
            self.report['compiler_graphs'][name] = graph
            self.inputs.update(graph['libraries'])

    def bind_headers(self, depfile, work):
        for path in dependencies(depfile, work):
            if path.is_file() and not path.is_relative_to(self.output):
                self.inputs[str(path)] = digest(path)

    def case(self, name, program, *, transport='source', optimization='O2', standard='c++20',
             relation='', mutant='', bias=0, expected_mismatch=None, probe=False, sanitized=False):
        work = self.output / name
        source_bundle(program, work, transport=transport, relation=relation, mutant=mutant, bias=bias,
            optimization=optimization, standard=standard, sanitized=sanitized, expected_mismatch=expected_mismatch)
        flags = [*self.flags, '-std='+standard, '-O2' if optimization == 'thinlto' else '-'+optimization,
                 *(['-flto=thin'] if optimization == 'thinlto' else [])]
        sanitizer_flags = []
        if sanitized:
            resources = Path(self.foundation['host_environment']['bootstrap_root']) / 'lib/clang/23'
            sanitizer_flags = ['-fsanitize=address,undefined', '-fno-sanitize-recover=undefined',
                               '-resource-dir', str(resources)]
            flags += sanitizer_flags
            for path in resources.rglob('libclang_rt.*'):
                if path.is_file() and any(kind in path.name for kind in ('asan', 'ubsan')):
                    self.inputs[str(path)] = digest(path)
        po, co, binary = work / 'provider.o', work / 'consumer.o', work / 'program.bin'
        provider_dep, consumer_dep = work / 'provider.d', work / 'consumer.d'
        if transport == 'interface':
            interface = work / 'oracle.Provider.objc3i'
            self.run(name+'-provider', [self.facade, work / 'provider.mm', '--objc3', '--arc', '--objcxx',
                '--emit-object', po, '--emit-interface', interface, '--module', 'oracle.Provider',
                '--package', 'semantic-oracle-v1', '--module-owned-names',
                '--module-source-root', work,
                *(part for item in EXPORTS for part in ('--export-declaration', item)),
                '--', *flags, '-MD', '-MF', provider_dep],
                outputs=[po, interface, interface.with_suffix('.objc3i.json')], stage='frontend-interface')
            self.bind_headers(provider_dep, work)
            module = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
            # Retain original evidence elsewhere; the consumer's physical input
            # directory contains no provider implementation or declaration header.
            retained = self.output / 'retained-provider' / name
            retained.mkdir(parents=True)
            for path in (work / 'provider.mm', work / 'oracle.h'):
                shutil.move(path, retained / path.name)
            command = [self.facade, work / 'consumer.mm', '--objc3', '--arc', '--objcxx',
                '--source-imports', '--module-search-path', work,
                '--import-module', 'oracle.Provider', module['semantic_fingerprint'], 'semantic-oracle-v1',
                '--emit-object', co, '--', *flags, '-MD', '-MF', consumer_dep,
                '-fmodules-cache-path='+str(work / 'fresh-module-cache')]
        else:
            self.run(name+'-provider', [self.tools['clang++'], *flags, '-MD', '-MF', provider_dep,
                '-c', work / 'provider.mm', '-o', po], outputs=[po])
            self.bind_headers(provider_dep, work)
            command = [self.tools['clang++'], *flags]
            if transport == 'pch':
                pch = work / 'oracle.pch'
                self.run(name+'-pch', [self.tools['clang++'], *flags, '-x', 'objective-c++-header',
                    work / 'oracle.h', '-o', pch], outputs=[pch], stage='frontend-pch')
                command += ['-include-pch', pch]
            command += ['-MD', '-MF', consumer_dep, '-c', work / 'consumer.mm', '-o', co]
        self.run(name+'-consumer', command, outputs=[co], stage='frontend-lowering')
        self.bind_headers(consumer_dep, work)
        self.run(name+'-link', [self.tools['clang++'], '--no-default-config', '-fuse-ld=lld',
            '-O2', *sanitizer_flags, *(['-flto=thin'] if optimization == 'thinlto' else []), po, co, self.services,
            self.runtime, '-pthread', '-Wl,-rpath,'+str(self.runtime.parent), '-o', binary], outputs=[binary])
        graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
            run=lambda suffix, argv: self.run(name+'-'+suffix, argv))
        self.inputs.update(graph['libraries'])
        output = self.run(name+'-execute', [binary], stage='runtime-oracle')
        try:
            actual = json.loads(output)
        except ValueError as error:
            raise StageFailure(name, 'output-schema', 'generated program did not produce its state/event record') from error
        classification = mismatch(interpret(program, bias=bias), actual)
        program_key = identity({'operations': [asdict(op) for op in program], 'bias': bias})
        reference = self.observations.setdefault(program_key, actual)
        row = {'name': name, 'transport': transport, 'optimization': optimization, 'standard': standard,
            'transform': relation, 'mutant': mutant, 'source_free': transport == 'interface',
            'generated_sanitizers': 'address,undefined' if sanitized else None,
            'mismatch': classification, 'binary_sha256': digest(binary), 'loaded_graph': graph,
            'oracle_disagreement': classification is not None, 'differential_disagreement': actual != reference,
            'model_sha256': digest(work / 'program.json'), 'status': 'PASS'}
        artifacts = [path for path in work.rglob('*') if path.is_file()]
        retained = self.output / 'retained-provider' / name
        if retained.is_dir():
            artifacts += list(retained.iterdir())
        row['artifacts'] = {str(path.relative_to(self.output)): digest(path) for path in artifacts}
        if transport == 'interface':
            row['module_identity'] = module
        if probe:
            row['status'] = 'MISMATCH' if classification else 'CLEAN'
            self.report.setdefault('reduction_trials', []).append(row)
            self.executor.save()
            return classification
        if classification != expected_mismatch:
            row['status'] = 'FAIL'
            self.report['cases'].append(row)
            self.executor.save()
            raise StageFailure(name, classification or 'missed-control', 'independent semantic oracle disagreed: '+str(classification))
        (self.report['controls'] if mutant else self.report['cases']).append(row)
        self.executor.save()
        return classification

    def stale_effect(self, program, old_fingerprint):
        name = 'control-stale-effect'
        work = self.output / name
        source_bundle(program, work, transport='interface')
        (work / 'oracle.h').write_text(HEADER.replace('ns_returns_retained', 'ns_returns_not_retained'))
        interface = work / 'oracle.Provider.objc3i'
        flags = [*self.flags, '-std=c++20', '-O0']
        self.run(name+'-provider', [self.facade, work / 'provider.mm', '--objc3', '--arc', '--objcxx',
            '--emit-interface', interface, '--module', 'oracle.Provider', '--package', 'semantic-oracle-v1',
            '--module-owned-names', '--module-source-root', work,
            *(part for item in EXPORTS for part in ('--export-declaration', item)), '--', *flags],
            outputs=[interface, interface.with_suffix('.objc3i.json')])
        changed = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']['semantic_fingerprint']
        if changed == old_fingerprint:
            raise ValueError('public ownership change did not change interface identity')
        retained = self.output / 'retained-provider' / name
        retained.mkdir(parents=True)
        for path in (work / 'provider.mm', work / 'oracle.h'):
            shutil.move(path, retained / path.name)
        self.run(name+'-reject', [self.facade, work / 'consumer.mm', '--objc3', '--arc', '--objcxx',
            '--source-imports', '--module-search-path', work, '--import-module', 'oracle.Provider',
            old_fingerprint, 'semantic-oracle-v1', '--emit-object', work / 'must-not-exist.o', '--', *flags],
            expected=1, contains=('O3IFC134',))
        if (work / 'must-not-exist.o').exists():
            raise ValueError('stale ownership contract produced a consumer object')
        self.report['controls'].append({'name': name, 'status': 'PASS', 'old': old_fingerprint, 'changed': changed})

    def reduce(self, program, *, mutant='', mismatch_class='value-or-lifetime',
               transport='source', optimization='O2', standard='c++20', relation='', bias=0):
        attempts = 0
        def interesting(trial):
            nonlocal attempts
            attempts += 1
            name = 'reduction-trial-'+str(attempts)
            try:
                return self.case(name, trial, mutant=mutant, probe=True, transport=transport,
                    optimization=optimization, standard=standard, relation=relation, bias=bias)
            except StageFailure as error:
                self.report.setdefault('reduction_rejections', []).append({'name': name,
                    'classification': error.classification, 'error': str(error)})
                return None
        reduced = shrink(program, interesting, mismatch_class)
        if mutant == 'tag' and len(reduced) >= len(program):
            raise ValueError('native semantic reduction removed no irrelevant operations')
        self.case('reduction-replay', reduced, mutant=mutant, expected_mismatch=mismatch_class,
            transport=transport, optimization=optimization, standard=standard, relation=relation, bias=bias)
        write_json(self.output / 'reduction.json', {'status': 'PASS', 'model_version': VERSION,
            'original': [asdict(op) for op in program], 'reduced': [asdict(op) for op in reduced],
            'mismatch': mismatch_class, 'attempts': attempts, 'transport': transport,
            'optimization': optimization, 'standard': standard, 'mutant': mutant, 'bias': bias,
            'transform': relation})

    def checked_case(self, name, program, **configuration):
        try:
            return self.case(name, program, **configuration)
        except StageFailure as error:
            if error.classification in ('value-or-lifetime', 'cleanup-order'):
                self.reduce(program, mismatch_class=error.classification, **configuration)
            raise


def qualify(repo: Path, output: Path, *, facade: Path, services: Path, runtime: Path, seeds=(7, 19, 41)):
    repo, output = repo.resolve(strict=True), output.resolve()
    if (output.exists() or not output.is_relative_to(repo / 'out') or not seeds or len(seeds) > 8 or
            len(set(seeds)) != len(seeds) or any(type(seed) is not int or not 0 <= seed < 2**32 for seed in seeds)):
        raise ValueError('semantic campaign requires fresh owned evidence and 1..8 seeds')
    output.mkdir(parents=True)
    campaign = None
    try:
        campaign = Campaign(repo, output, facade, services, runtime)
        expected = []
        for seed in seeds:
            program = generate(seed)
            for transport in ('source', 'pch', 'interface'):
                for optimization in ('O0', 'O2'):
                    name = f'seed-{seed}-{transport}-{optimization}'
                    expected.append(name)
                    campaign.checked_case(name, program, transport=transport, optimization=optimization)
            for relation in ('rename', 'comments', 'pure-grouping'):
                name = f'seed-{seed}-{relation}'
                expected.append(name)
                campaign.checked_case(name, program, relation=relation, standard='c++23')
            name = f'seed-{seed}-thinlto'
            expected.append(name)
            campaign.checked_case(name, program, optimization='thinlto')
        for mutant in ('tag', 'defer', 'destructor', 'provider', 'error'):
            campaign.case('control-'+mutant, generate(seeds[0]), mutant=mutant, sanitized=True,
                expected_mismatch='cleanup-order' if mutant in ('defer', 'destructor') else 'value-or-lifetime')
        expected.append('sanitizer-clean-control')
        campaign.case('sanitizer-clean-control', generate(seeds[0]), sanitized=True)
        for bias in (0, 1):
            name = 'private-body-'+str(bias)
            expected.append(name)
            campaign.case(name, generate(seeds[0]), transport='interface', bias=bias)
        base, changed = campaign.report['cases'][-2:]
        if base['module_identity'] != changed['module_identity']:
            raise ValueError('private implementation edit changed public interface identity')
        campaign.stale_effect(generate(seeds[0]), base['module_identity']['semantic_fingerprint'])
        campaign.reduce(generate(seeds[0]), mutant='tag')
        if {row['name'] for row in campaign.report['cases']} != set(expected):
            raise ValueError('generated semantic execution inventory is incomplete')
        if any(digest(Path(path)) != signature for path, signature in campaign.inputs.items()):
            raise ValueError('semantic campaign input changed during execution')
        campaign.report.update(status='PASS', expected_cases=expected,
            seeds=list(seeds), semantic_coverage=coverage([generate(seed) for seed in seeds]),
            metrics={'commands': len(campaign.report['commands']),
                'native_compilations': sum(row['stage'].startswith('frontend') for row in campaign.report['commands']),
                'command_seconds': round(sum(row['seconds'] for row in campaign.report['commands']), 6)})
    except (OSError, ValueError) as error:
        if campaign is None:
            receipt = output / 'qualification.json'
            report = json.loads(receipt.read_text()) if receipt.exists() else {}
            report.update(status='FAIL', error=str(error), classification='infrastructure')
            write_json(receipt, report)
        else:
            campaign.report.update(status='FAIL', error=str(error), classification=getattr(error, 'classification', 'infrastructure'))
        raise
    finally:
        if campaign is not None:
            campaign.executor.save()
    return campaign.report


def replay(repo: Path, output: Path, document: Path, *, facade: Path, services: Path, runtime: Path):
    record = json.loads(document.read_text())
    if record.get('model_version') != VERSION:
        raise ValueError('semantic replay requires its exact model/generator version')
    program = [Operation(**item) for item in record.get('reduced', record.get('operations', []))]
    validate(program)
    repo, output = repo.resolve(strict=True), output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('semantic replay needs fresh owned evidence')
    output.mkdir(parents=True)
    campaign = None
    try:
        campaign = Campaign(repo, output, facade, services, runtime)
        campaign.report['subject'] = 'semantic-failure-replay'
        campaign.case('replay', program, transport=record.get('transport', 'source'),
            optimization=record.get('optimization', 'O2'), standard=record.get('standard', 'c++20'),
            relation=record.get('transform', ''), mutant=record.get('mutant', ''),
            bias=record.get('bias', record.get('provider_bias', 0)), sanitized=record.get('sanitized', False),
            expected_mismatch=record.get('mismatch', record.get('expected_mismatch')))
        if any(digest(Path(path)) != signature for path, signature in campaign.inputs.items()):
            raise ValueError('semantic replay input changed during execution')
        campaign.report.update(status='PASS', reproduced_mismatch=record.get('mismatch', record.get('expected_mismatch')))
    except (OSError, ValueError) as error:
        if campaign is None:
            receipt = output / 'qualification.json'
            report = json.loads(receipt.read_text()) if receipt.exists() else {}
            report.update(status='FAIL', error=str(error), classification='infrastructure')
            write_json(receipt, report)
        else:
            campaign.report.update(status='FAIL', error=str(error))
        raise
    finally:
        if campaign is not None:
            campaign.executor.save()
    return campaign.report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--facade', type=Path)
    parser.add_argument('--services', type=Path)
    parser.add_argument('--runtime', type=Path)
    parser.add_argument('--ir-receipt', type=Path, help='use a complete matched IR campaign tool/runtime bundle')
    parser.add_argument('--program-json', type=Path, help='replay a retained typed program or reduction')
    parser.add_argument('--seeds', nargs='+', type=int, default=[7, 19, 41])
    parser.add_argument('--seed-rotation', default='', help='add three deterministic seeds bound to a tested revision')
    args = parser.parse_args(argv)
    if args.ir_receipt:
        ir = json.loads(args.ir_receipt.read_text())
        tool_receipt = json.loads((args.ir_receipt.parent / 'tools.json').read_text())
        plan, tools, _ = selected(args.repo.resolve(strict=True))
        verify_receipt(tool_receipt, plan=plan, tools=tools, repo=args.repo.resolve(strict=True))
        facades = [Path(path) for path in ir.get('inputs', {}) if path.endswith('/src/clang_foundation/objc3c')]
        if ir.get('status') != 'PASS' or ir.get('tool_identity') != tool_receipt['identity'] or len(facades) != 1:
            raise ValueError('semantic oracles require a complete matched IR receipt')
        args.facade, args.services, args.runtime = facades[0], Path(ir['runtime']['service_archive']), Path(ir['runtime']['objc_runtime'])
        for path in (args.facade, args.services, args.runtime):
            if digest(path) != ir['inputs'][str(path)]:
                raise ValueError('semantic IR input changed: '+str(path))
    if not all((args.facade, args.services, args.runtime)):
        parser.error('provide --ir-receipt or explicit --facade/--services/--runtime')
    if args.program_json:
        result = replay(args.repo, args.output, args.program_json, facade=args.facade,
                        services=args.services, runtime=args.runtime)
    else:
        result = qualify(args.repo, args.output, facade=args.facade, services=args.services,
                         runtime=args.runtime, seeds=seed_bank(args.seeds, args.seed_rotation))
    print(json.dumps({'status': result['status'], 'cases': len(result['cases']), 'controls': len(result['controls'])}))


if __name__ == '__main__':
    main()
