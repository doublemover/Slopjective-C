"""Compare bounded generated task/group/actor histories with an independent model."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import time

from .binary_packages import digest, write_json
from .product_evidence import ProductCommands
from .runtime_instrumentation import RuntimeProfiles, used_receipts
from .runtime_schedule_model import (OPCODES, SPLIT_EPOCH_VERSION, Model, check, coverage_domain,
    generate, generate_split_epoch, protocol, signature, shrink, validate)
from .sanitizer_policy import SanitizerQualification


DEFAULT_SEEDS = (0, 1, 2, 3, 7, 19, 41, 63)
EXPLORATION_SEEDS = range(64, 128)
EXPLORATION_LIMIT = 12
SCHEMA = 'objc3c.generated-runtime-schedules.v2'
BACKENDS = ('manual', 'manual-repeat', 'worker')
SANITIZERS = ('address,undefined', 'thread')
OPTIMIZATIONS = ('O0', 'O2')
SEMANTIC_CONTROLS = {
    1: ('missing-frame-destruction', 'runtime semantic/lifetime mismatch at drain'),
    2: ('duplicate-waiter-delivery', 'runtime semantic/lifetime mismatch at drain'),
    3: ('premature-quiescence-publication', 'runtime semantic/lifetime mismatch at drain'),
    4: ('overlapping-actor-jobs', 'runtime semantic/lifetime mismatch at drain'),
    5: ('type-mismatched-next-consumption', 'completion-order result was absent or consumed twice'),
}
SCENARIO_INVENTORY = {
    'concurrent-registration-cancellation-publication': 'task-services',
    'interrupted-group-waiter-registration-retirement': 'task-services',
    'interrupted-dependency-waiter-registration-retirement': 'task-services',
    'transitive-task-dependency-cycle-rejection': 'task-services',
    'mixed-task-and-closed-group-dependency-cycles': 'task-services',
    'implicit-scope-membership-dependency-cycles': 'task-services',
    'forced-scope-join-retires-interrupted-body-await': 'task-services',
    'global-pool-multi-actor-and-task-exclusion': 'task-services',
    'queued-discard-and-named-binding-retirement': 'task-services',
    'nested-and-last-owner-abandoned-awaits': 'structured-runtime',
    'typed-next-mismatch-nonconsumption': 'async-handle-joins',
    'allocation-publication-and-copy-failure-injection': 'async-handle-joins',
    'dependency-wake-allocation-failure-and-shared-result': 'async-handle-joins',
    'deep-scope-cleanup-with-failed-allocation': 'async-handle-joins',
    'unforced-worker-group-and-actor-stress': 'structured-runtime',
    'executor-last-release-and-worker-drain': 'runtime-lifetime',
    'reentrant-result-copy-lease': 'runtime-lifetime',
    'callable-native-frame-admission': 'runtime-lifetime',
    'constrained-stack-task-group-depth': 'runtime-lifetime',
    'constrained-stack-dependency-wake-rejection': 'runtime-lifetime',
    'actor-object-generation-reuse-and-competing-first-entry-arc': 'actor-lifetime-arc',
    'actor-object-generation-reuse-and-competing-first-entry-mrc': 'actor-lifetime-mrc',
    'object-block-void-and-error-results-arc': 'actor-lifetime-arc',
    'object-block-void-and-error-results-mrc': 'actor-lifetime-mrc',
}


def observations(text):
    if len(text) > 256*1024: raise ValueError('runtime schedule trace exceeded its byte bound')
    try: rows = [json.loads(line) for line in text.splitlines()]
    except ValueError as error: raise ValueError('malformed runtime schedule observation') from error
    if not rows or len(rows) > 96 or any(not isinstance(row, dict) for row in rows):
        raise ValueError('runtime schedule observation omitted its bounded operation ledger')
    return rows


def watchdog_snapshot(command, argv):
    snapshot = {'name': command['name'], 'log': command['log'], 'status': 'timeout'}
    if len(argv) != 3 or argv[1] not in ('manual', 'worker'):
        return snapshot
    operations = json.loads(Path(argv[2]).read_text())
    with Path(command['log']).open(errors='replace') as stream:
        lines = stream.read(256 * 1024).splitlines()
    completed = []
    for line, (opcode, slot) in zip(lines, operations):
        try:
            event = json.loads(line)
        except ValueError:
            break
        if not isinstance(event, dict) or (event.get('operation'), event.get('id')) != (OPCODES[opcode], slot):
            break
        completed.append(event)
    blocked = len(completed)
    pending = ['final-reclamation', None]
    if blocked < len(operations):
        opcode, slot = operations[blocked]
        pending = [OPCODES[opcode], slot]
    return {**snapshot, 'backend': argv[1], 'completed_operations': blocked,
            'pending_operation': pending, 'last_events': completed[-3:]}


def select_exploration(limit):
    if type(limit) is not int or not 1 <= limit <= EXPLORATION_LIMIT:
        raise ValueError(f'exploration history budget must be between 1 and {EXPLORATION_LIMIT}')
    fixed_transitions, fixed_pairs = coverage_domain()
    candidates, rejected, protocols = [], [], set()
    candidate_transitions, candidate_pairs, candidate_cells = set(), set(), set()
    for seed in EXPLORATION_SEEDS:
        history = generate_split_epoch(seed)
        gate = 'before-registration' if seed % 2 == 0 else 'after-registration'
        encoded = protocol(history).encode()
        protocol_sha256 = hashlib.sha256(encoded).hexdigest()
        identity = {'seed': seed, 'gate': gate, 'history_sha256': signature(history),
                    'group_cancellation': ['cancel-group', 0] in history['operations'],
                    'actor_cancellation': ['cancel-actor', 0] in history['operations'],
                    'protocol_sha256': protocol_sha256, 'operations': len(history['operations'])}
        if protocol_sha256 in protocols:
            rejected.append({**identity, 'reason': 'duplicate-operation-protocol'})
            continue
        protocols.add(protocol_sha256)
        model = Model()
        model.run(history['operations'])
        candidate_transitions.update(model.transitions)
        candidate_pairs.update(model.pairs)
        cell = (gate, identity['group_cancellation'], identity['actor_cancellation'])
        candidate_cells.add(cell)
        candidates.append((identity, history, model.transitions, model.pairs, cell))

    transitions, pairs, gates, cells = set(fixed_transitions), set(fixed_pairs), set(), set()
    histories, selected = [], []
    while len(histories) < limit and candidates:
        def rank(candidate):
            identity, _, labels, adjacent, cell = candidate
            return (identity['gate'] not in gates, len(labels - transitions),
                    len(adjacent - pairs), cell not in cells, -identity['seed'])

        winner = max(candidates, key=rank)
        candidates.remove(winner)
        identity, history, labels, adjacent, cell = winner
        selected.append({**identity, 'added_transitions': sorted(labels - transitions),
                         'added_pairs': sorted(map(list, adjacent - pairs))})
        histories.append(history)
        gates.add(identity['gate'])
        cells.add(cell)
        transitions.update(labels)
        pairs.update(adjacent)
    rejected.extend({**identity, 'reason': 'history-budget'} for identity, *_ in candidates)
    return histories, {
        'mode': 'bounded-split-epoch',
        'budget': {'candidate_seeds': [EXPLORATION_SEEDS.start, EXPLORATION_SEEDS.stop - 1],
                   'candidates': len(EXPLORATION_SEEDS), 'histories': limit,
                   'native_cases': limit * len(SANITIZERS) * len(OPTIMIZATIONS) * len(BACKENDS)},
        'selected': selected, 'rejected': sorted(rejected, key=lambda row: row['seed']),
        'candidate_coverage': {'transitions': sorted(candidate_transitions),
                               'pairs': sorted(map(list, candidate_pairs)),
                               'gate_and_cancellation_combinations': sorted(map(list, candidate_cells))},
    }


def qualify(repo, compiler_root, resource_root, output, *, seeds=DEFAULT_SEEDS, replay=None,
            explore=False, explore_limit=None):
    import resource

    repo, compiler_root, resource_root = (path.resolve(strict=True) for path in (repo, compiler_root, resource_root))
    output = output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('runtime schedule qualification requires fresh owned evidence')
    if (explore and (replay or seeds != DEFAULT_SEEDS)) or (explore_limit is not None and not explore):
        raise ValueError('use exploration, fixed seeds, or replay as separate selections')
    exploration = None
    if explore:
        histories, exploration = select_exploration(EXPLORATION_LIMIT if explore_limit is None else explore_limit)
    else:
        histories = [json.loads(replay.read_text())] if replay else [generate(seed) for seed in seeds]
    for value in histories: validate(value)
    if not histories or len(histories) > 16 or len({signature(h) for h in histories}) != len(histories):
        raise ValueError('runtime schedule selection must be bounded, nonempty and unique')
    generated_only = explore or bool(replay and histories[0]['model_version'] == SPLIT_EPOCH_VERSION)
    if generated_only and replay:
        exploration = {'mode': 'split-epoch-replay', 'original_history_sha256': signature(histories[0])}
    output.mkdir(parents=True)
    work = output / 'work';work.mkdir()
    fixture = repo / 'tests/clang_foundation/runtime_schedules/history.cpp'
    lifetime = repo / 'native/objc3c/tests/runtime_lifetime_test.cpp'
    actor_fixtures = repo / 'tests/clang_foundation/native_async'
    scenario_sources = (
        repo / 'tests/tooling/fixtures/clang_foundation/services/task_services.cpp',
        repo / 'tests/clang_foundation/structured_runtime/actor_scheduling.cpp',
        repo / 'tests/clang_foundation/structured_runtime/worker.cpp',
        repo / 'tests/clang_foundation/async_handle_joins/runtime.cpp',
        repo / 'native/objc3c/src/runtime/concurrency/service_group.cpp',
        actor_fixtures / 'provider.cpp', actor_fixtures / 'provider.h',
        actor_fixtures / 'actor_lifetime_control.mm', lifetime)
    model_source = Path(__file__).with_name('runtime_schedule_model.py')
    protocol_hashes = []
    for index, history in enumerate(histories):
        write_json(work / ('history-'+str(index)+'.json'), history)
        native_protocol = work / ('protocol-'+str(index)+'.json')
        native_protocol.write_text(protocol(history))
        protocol_hashes.append(digest(native_protocol))
    # A valid user replay may omit the transition that a particular mutant
    # exercises. Keep the negative controls on their known witness history.
    control_protocol = work / 'control-protocol.json'
    if not generated_only:
        control_history = generate(DEFAULT_SEEDS[0])
        write_json(work / 'control-history.json', control_history)
        control_protocol.write_text(protocol(control_history))
    products = profiles = None
    report = {'schema': SCHEMA, 'status': 'running', 'commands': [],
        'histories': histories, 'history_hashes': list(map(signature, histories)), 'cases': [], 'controls': [],
        'scope': ('Bounded split-epoch generated public-C histories.' if generated_only else
                  'Bounded generated public-C histories plus current public-contract scenario fixtures. Manual/gated replay is complemented by unforced worker/global-pool executions; this is not exhaustive scheduler or memory-model verification.'),
        'scenario_inventory': {} if generated_only else SCENARIO_INVENTORY,
        'bounds': {'generated_operations': 96, 'generated_tasks': 8, 'next_probe_tasks': 1, 'generated_actors': 1,
                   'overlap_control_threads': 1, 'history_timeout_seconds': 30,
                   'worker_fixture_timeout_seconds': 90, 'native_reduction_trials': 32},
        'child_rss_before_kib': resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss}
    if generated_only:
        report.update(exploration=exploration, protocol_hashes=protocol_hashes)
        report['bounds']['worker_gates'] = 2
    else:
        report['control_history_hash'] = signature(control_history)
    tick = time.monotonic()
    def save():
        report['seconds'] = round(time.monotonic()-tick, 6)
        report['peak_child_rss_kib'] = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
        write_json(output / 'qualification.json', report)
    def run(name, argv, *, timeout=30):
        try:
            row = products.record(policy, name, argv, report['commands'], cwd=work, timeout=timeout)
        except ValueError:
            row = report['commands'][-1]
            if row.get('timed_out'):
                report['watchdog'] = watchdog_snapshot(row, argv)
            raise
        return Path(row['log']).read_text(errors='replace')
    try:
        products = ProductCommands(output / 'product-evidence',
            sources=(fixture, model_source, Path(__file__), work, *(() if generated_only else scenario_sources)))
        profiles = RuntimeProfiles(repo, compiler_root, resource_root, output / 'runtime-profiles')
        transition_hits, pair_hits = set(), set()
        for sanitizer in SANITIZERS:
            runtime = profiles.get(sanitizer, 'O1')
            for optimization in OPTIMIZATIONS:
                label = sanitizer.replace(',', '-')+'-'+optimization
                policy = SanitizerQualification(compiler_root / 'bin/clang++', resource_root,
                    output / 'detector-controls' / label, sanitizer,
                    libraries=(runtime.archive, runtime.objc_runtime), additional_inputs=(Path(__file__), model_source))
                policy.ensure_controls(optimization)
                common = [compiler_root / 'bin/clang++', '--no-default-config', '-std=c++20', '-'+optimization,
                    '-g', '-UNDEBUG', '-Wall', '-Wextra', '-Werror', '-pthread', '-fno-omit-frame-pointer',
                    '-fno-sanitize-recover=undefined', '-fsanitize='+sanitizer, '-resource-dir', resource_root,
                    '-I'+str(repo / 'native/objc3c/src/runtime/public')]
                def build(suffix, mutant=0):
                    obj, binary = work / (suffix+'.o'), work / suffix
                    run(suffix+'-compile', [*common, '-DSCHEDULE_MUTANT='+str(mutant), '-c', fixture, '-o', obj])
                    run(suffix+'-link', [*common, '-fuse-ld=lld', obj, runtime.archive, '-Wl,--no-as-needed',
                        runtime.objc_runtime, '-Wl,--as-needed', '-Wl,-rpath,'+str(runtime.objc_runtime.parent), '-o', binary])
                    return binary
                def build_fixture(suffix, sources, *, definitions=(), compile_options=(), link_options=()):
                    objects = []
                    for source_index, source in enumerate(sources):
                        obj = work / (suffix+'-'+str(source_index)+'.o')
                        run(suffix+'-'+str(source_index)+'-compile', [*common,
                            *('-D'+value for value in definitions), '-I'+str(repo / 'native/objc3c/src'),
                            *compile_options, '-c', source, '-o', obj], timeout=120)
                        objects.append(obj)
                    binary = work / suffix
                    run(suffix+'-link', [*common, '-fuse-ld=lld', *objects, runtime.archive,
                        *link_options, '-Wl,--no-as-needed', runtime.objc_runtime, '-Wl,--as-needed',
                        '-Wl,-rpath,'+str(runtime.objc_runtime.parent), '-o', binary], timeout=120)
                    return binary
                binary = build(label)
                for index, history in enumerate(histories):
                    manual = None
                    for backend in BACKENDS:
                        actual_backend = 'manual' if backend.startswith('manual') else 'worker'
                        name = label+'-history-'+str(index)+'-'+backend
                        text = run(name+'-execute', [binary, actual_backend, work / ('protocol-'+str(index)+'.json')])
                        observed = observations(text)
                        result = check(history, observed)
                        if backend == 'manual': manual = observed
                        elif backend == 'manual-repeat' and observed != manual:
                            raise ValueError('manual runtime schedule replay changed its logical trace')
                        transition_hits.update(result['transitions']);pair_hits.update(map(tuple, result['adjacent_pairs']))
                        report['cases'].append({'name': name, 'status': 'PASS', 'history': report['history_hashes'][index],
                            'backend': backend, 'sanitizer': sanitizer, 'client_optimization': optimization,
                            'service_optimization': 'O1', 'observation_sha256': digest(Path(report['commands'][-1]['log'])), **result})
                        if generated_only:
                            report['cases'][-1]['protocol_sha256'] = protocol_hashes[index]
                        save()
                if generated_only:
                    report.setdefault('sanitizer_policy', {})[label] = policy.finalize()
                    continue
                if sanitizer == 'address,undefined' and optimization == 'O0':
                    for mutant, (kind, expected_failure) in SEMANTIC_CONTROLS.items():
                        bad = build(label+'-mutant-'+str(mutant), mutant)
                        text = run(label+'-mutant-'+str(mutant)+'-execute', [bad, 'manual', control_protocol])
                        observed = observations(text)
                        try: check(control_history, observed)
                        except ValueError as error:
                            if str(error) != expected_failure:
                                raise ValueError('runtime control failed outside its expected transition: '+kind) from error
                            report['controls'].append({'name': kind, 'status': 'PASS', 'classification': 'semantic-mismatch',
                                'diagnostic': str(error), 'native_exit': 0, 'sanitizer_clean': True})
                        else: raise ValueError('runtime semantic model accepted its wrong-code control')
                        if mutant == 1:
                            attempts = []
                            def reproduces(candidate):
                                index = len(attempts)
                                path = work / ('reduction-'+str(index)+'.json');path.write_text(protocol(candidate))
                                text = run(label+'-reduction-'+str(index)+'-execute', [bad, 'manual', path])
                                observed = observations(text)
                                try: check(candidate, observed)
                                except ValueError as error: reproduced = str(error) == expected_failure
                                else: reproduced = False
                                attempts.append({'history': candidate, 'reproduced': reproduced})
                                return reproduced
                            reduced, budget = shrink(control_history, reproduces)
                            fresh = work / 'fresh-reduced-replay';fresh.mkdir()
                            shutil.copy2(bad, fresh / 'control')
                            path = fresh / 'protocol.json';path.write_text(protocol(reduced))
                            text = run(label+'-fresh-reduced-replay-execute', [fresh / 'control', 'manual', path])
                            observed = observations(text)
                            try: check(reduced, observed)
                            except ValueError as error:
                                if str(error) != expected_failure: raise ValueError('reduced replay changed failure class') from error
                            else: raise ValueError('reduced runtime history failed to reproduce')
                            report['reduction'] = {'status': 'PASS', 'original': control_history, 'reduced': reduced,
                                'attempts': attempts, 'budget': budget, 'failure_signature': expected_failure,
                                'fresh_native_replay': True}
                fixtures = {
                    'task-services': build_fixture(label+'-task-services', [
                        repo / 'tests/tooling/fixtures/clang_foundation/services/task_services.cpp',
                        repo / 'tests/clang_foundation/structured_runtime/actor_scheduling.cpp',
                        repo / 'native/objc3c/src/runtime/concurrency/service_group.cpp'],
                        definitions=('OBJC3_RUNTIME_SERVICE_TESTING',)),
                    'structured-runtime': build_fixture(label+'-structured-runtime', [
                        repo / 'tests/clang_foundation/structured_runtime/worker.cpp']),
                    'async-handle-joins': build_fixture(label+'-async-handle-joins', [
                        repo / 'tests/clang_foundation/async_handle_joins/runtime.cpp'],
                        link_options=('-Wl,--wrap=_ZnwmRKSt9nothrow_t',
                                      '-Wl,--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t')),
                    'runtime-lifetime': build_fixture(label+'-runtime-lifetime', [lifetime])}
                for ownership, flag in (('arc', '-fobjc-arc'), ('mrc', '-fno-objc-arc')):
                    owner = 'actor-lifetime-'+ownership
                    fixtures[owner] = build_fixture(label+'-'+owner,
                        [actor_fixtures / 'provider.cpp', actor_fixtures / 'actor_lifetime_control.mm'],
                        definitions=('OBJC3_ACTOR_LIFETIME_INTERNAL_TESTING',),
                        compile_options=('-x', 'objective-c++', '-fblocks', '-fobjc-version=3', flag,
                                         '-fcoro-aligned-allocation', '-fobjc-runtime=gnustep-2.2',
                                         '-Wno-unused-parameter'),
                        link_options=('-Wl,--wrap=_Znwm',))
                for fixture_name, fixture_binary in fixtures.items():
                    text = run(label+'-'+fixture_name+'-execute', [fixture_binary], timeout=90)
                    if fixture_name == 'task-services' and 'task services: all focused checks passed' not in text:
                        raise ValueError('task-service scenario fixture omitted its completion witness')
                    if fixture_name == 'runtime-lifetime' and 'runtime lifetime: all focused checks passed' not in text:
                        raise ValueError('runtime lifetime scenario fixture omitted its completion witness')
                    if fixture_name.startswith('actor-lifetime-') and 'actor lifetime: all focused checks passed' not in text:
                        raise ValueError('actor lifetime scenario fixture omitted its completion witness')
                    report.setdefault('scenario_cases', []).append({'name': label+'-'+fixture_name,
                        'fixture': fixture_name, 'status': 'PASS', 'sanitizer': sanitizer,
                        'client_optimization': optimization, 'service_optimization': 'O1',
                        'observation_sha256': digest(Path(report['commands'][-1]['log'])),
                        'scenarios': sorted(name for name, owner in report['scenario_inventory'].items()
                                            if owner == fixture_name)})
                    save()
                report.setdefault('sanitizer_policy', {})[label] = policy.finalize()
        configurations = len(SANITIZERS)*len(OPTIMIZATIONS)
        if len(report['cases']) != len(histories)*configurations*len(BACKENDS):
            raise ValueError('generated runtime schedule inventory is incomplete')
        if not generated_only and (
                len(report['controls']) != len(SEMANTIC_CONTROLS) or
                len(report.get('scenario_cases', [])) != configurations*len(set(SCENARIO_INVENTORY.values())) or
                {scenario for row in report['scenario_cases'] for scenario in row['scenarios']} !=
                set(report['scenario_inventory'])):
            raise ValueError('runtime schedule configuration/control inventory is incomplete')
        domain, pair_domain = coverage_domain()
        report['model_coverage'] = {'denominator': 'Reviewed 64-seed bounded grammar; not all public API states or implementation coverage.',
            'transition_domain': sorted(domain), 'pair_domain': sorted(map(list, pair_domain)),
            'missing_transitions': sorted(domain-transition_hits),
            'missing_pairs': sorted(map(list, pair_domain-pair_hits))}
        if not replay and not generated_only and (domain-transition_hits or pair_domain-pair_hits):
            raise ValueError('fixed runtime schedule seeds omitted reviewed grammar transitions/pairs')
        if generated_only:
            report['exploration']['coverage'] = {
                'fixed_transition_count': len(domain), 'fixed_pair_count': len(pair_domain),
                'fixed_history_hashes': [signature(generate(seed)) for seed in DEFAULT_SEEDS],
                'added_transitions': sorted(transition_hits - domain),
                'added_pairs': sorted(map(list, pair_hits - pair_domain)),
            }
            if explore:
                candidate = exploration['candidate_coverage']
                visited = {(row['gate'], row['group_cancellation'], row['actor_cancellation'])
                           for row in exploration['selected']}
                report['exploration']['coverage'].update(
                    unvisited_candidate_transitions=sorted(set(candidate['transitions']) - transition_hits),
                    unvisited_candidate_pairs=sorted(map(list, set(map(tuple, candidate['pairs'])) - pair_hits)),
                    visited_gate_and_cancellation_combinations=sorted(map(list, visited)),
                    unvisited_gate_and_cancellation_combinations=sorted(map(list,
                        set(map(tuple, candidate['gate_and_cancellation_combinations'])) - visited)))
        report.update(status='PASS', runtime_profiles=used_receipts(profiles, ('O1',)),
            transitions=sorted(transition_hits), adjacent_pairs=sorted(map(list, pair_hits)),
            product_evidence=products.finalize())
    except Exception as error:
        report.update(status='FAIL', error=str(error));raise
    finally: save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('compiler-root', 'resource-root', 'output'):
        parser.add_argument('--'+name, type=Path, required=True)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument('--replay', type=Path)
    selection.add_argument('--explore', action='store_true', help='run bounded additional split-epoch histories')
    parser.add_argument('--explore-limit', type=int, help='cap exploration at 1 to 12 selected histories')
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'cases': len(result['cases'])}))


if __name__ == '__main__': main()
