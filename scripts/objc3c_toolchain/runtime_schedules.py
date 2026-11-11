"""Compare bounded generated task/group/actor histories with an independent model."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import time

from .binary_packages import digest, write_json
from .product_evidence import ProductCommands
from .runtime_instrumentation import RuntimeProfiles, used_receipts
from .runtime_schedule_model import check, coverage_domain, generate, protocol, signature, shrink, validate
from .sanitizer_policy import SanitizerQualification


def observations(text):
    if len(text) > 256*1024: raise ValueError('runtime schedule trace exceeded its byte bound')
    try: rows = [json.loads(line) for line in text.splitlines()]
    except ValueError as error: raise ValueError('malformed runtime schedule observation') from error
    if not rows or len(rows) > 96 or any(not isinstance(row, dict) for row in rows):
        raise ValueError('runtime schedule observation omitted its bounded operation ledger')
    return rows


def qualify(repo, compiler_root, resource_root, output, *, seeds=(0, 1, 2, 3, 7, 19, 41, 63), replay=None):
    repo, compiler_root, resource_root = (path.resolve(strict=True) for path in (repo, compiler_root, resource_root))
    output = output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('runtime schedule qualification requires fresh owned evidence')
    histories = [json.loads(replay.read_text())] if replay else [generate(seed) for seed in seeds]
    for value in histories: validate(value)
    if not histories or len(histories) > 16 or len({signature(h) for h in histories}) != len(histories):
        raise ValueError('runtime schedule selection must be bounded, nonempty and unique')
    output.mkdir(parents=True)
    work = output / 'work';work.mkdir()
    fixture = repo / 'tests/clang_foundation/runtime_schedules/history.cpp'
    model_source = Path(__file__).with_name('runtime_schedule_model.py')
    for index, history in enumerate(histories):
        write_json(work / ('history-'+str(index)+'.json'), history)
        (work / ('protocol-'+str(index)+'.json')).write_text(protocol(history))
    products = profiles = None
    report = {'schema': 'objc3c.generated-runtime-schedules.v1', 'status': 'running', 'commands': [],
        'histories': histories, 'history_hashes': list(map(signature, histories)), 'cases': [], 'controls': [],
        'scope': 'Bounded generated public-C histories plus current public-contract scenario fixtures. Manual/gated replay is complemented by unforced worker/global-pool executions; this is not exhaustive scheduler or memory-model verification.',
        'scenario_inventory': {
            'concurrent-registration-cancellation-publication': 'task-services',
            'global-pool-multi-actor-and-task-exclusion': 'task-services',
            'queued-discard-and-named-binding-retirement': 'task-services',
            'nested-and-last-owner-abandoned-awaits': 'structured-runtime',
            'typed-next-mismatch-nonconsumption': 'async-handle-joins',
            'allocation-publication-and-copy-failure-injection': 'async-handle-joins',
            'unforced-worker-group-and-actor-stress': 'structured-runtime'},
        'bounds': {'generated_operations': 96, 'generated_tasks': 8, 'generated_actors': 1,
                   'worker_fixture_timeout_seconds': 90, 'native_reduction_trials': 32}}
    tick = time.monotonic()
    def save():
        report['seconds'] = round(time.monotonic()-tick, 6);write_json(output / 'qualification.json', report)
    def run(name, argv, *, timeout=30):
        row = products.record(policy, name, argv, report['commands'], cwd=work, timeout=timeout)
        return Path(row['log']).read_text(errors='replace')
    try:
        products = ProductCommands(output / 'product-evidence', sources=(fixture, model_source, Path(__file__), work))
        profiles = RuntimeProfiles(repo, compiler_root, resource_root, output / 'runtime-profiles')
        transition_hits, pair_hits = set(), set()
        for sanitizer in ('address,undefined', 'thread'):
            runtime = profiles.get(sanitizer, 'O1')
            for optimization in ('O0', 'O2'):
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
                def build_fixture(suffix, sources, *, definitions=(), link_options=()):
                    objects = []
                    for source_index, source in enumerate(sources):
                        obj = work / (suffix+'-'+str(source_index)+'.o')
                        run(suffix+'-'+str(source_index)+'-compile', [*common,
                            *('-D'+value for value in definitions), '-I'+str(repo / 'native/objc3c/src'),
                            '-c', source, '-o', obj], timeout=120)
                        objects.append(obj)
                    binary = work / suffix
                    run(suffix+'-link', [*common, '-fuse-ld=lld', *objects, runtime.archive,
                        *link_options, '-Wl,--no-as-needed', runtime.objc_runtime, '-Wl,--as-needed',
                        '-Wl,-rpath,'+str(runtime.objc_runtime.parent), '-o', binary], timeout=120)
                    return binary
                binary = build(label)
                for index, history in enumerate(histories):
                    manual = None
                    for backend in ('manual', 'manual-repeat', 'worker'):
                        actual_backend = 'manual' if backend.startswith('manual') else 'worker'
                        name = label+'-history-'+str(index)+'-'+backend
                        text = run(name+'-execute', [binary, actual_backend, work / ('protocol-'+str(index)+'.json')])
                        observed = observations(text)
                        result = check(history, observed)
                        if backend == 'manual': manual = observed
                        elif backend == 'manual-repeat' and observed != manual:
                            raise ValueError('manual runtime schedule replay changed its logical trace')
                        transition_hits.update(result['transitions']);pair_hits.update(map(tuple, result['adjacent_pairs']))
                        report['cases'].append({'name': name, 'status': 'PASS', 'history': signature(history),
                            'backend': backend, 'sanitizer': sanitizer, 'client_optimization': optimization,
                            'service_optimization': 'O1', 'observation_sha256': digest(Path(report['commands'][-1]['log'])), **result})
                        save()
                if sanitizer == 'address,undefined' and optimization == 'O0':
                    for mutant, kind in ((1, 'missing-frame-destruction'), (2, 'duplicate-waiter-delivery'),
                                         (3, 'premature-quiescence-publication'), (4, 'overlapping-actor-jobs'),
                                         (5, 'type-mismatched-next-consumption')):
                        bad = build(label+'-mutant-'+str(mutant), mutant)
                        text = run(label+'-mutant-'+str(mutant)+'-execute', [bad, 'manual', work / 'protocol-0.json'])
                        try: check(histories[0], observations(text))
                        except ValueError as error:
                            report['controls'].append({'name': kind, 'status': 'PASS', 'classification': 'semantic-mismatch',
                                'diagnostic': str(error), 'native_exit': 0, 'sanitizer_clean': True})
                            failure_signature = str(error)
                        else: raise ValueError('runtime semantic model accepted its wrong-code control')
                        if mutant == 1:
                            attempts = []
                            def reproduces(candidate):
                                index = len(attempts)
                                path = work / ('reduction-'+str(index)+'.json');path.write_text(protocol(candidate))
                                text = run(label+'-reduction-'+str(index)+'-execute', [bad, 'manual', path])
                                try: check(candidate, observations(text))
                                except ValueError as error: reproduced = str(error) == failure_signature
                                else: reproduced = False
                                attempts.append({'history': candidate, 'reproduced': reproduced})
                                return reproduced
                            reduced, budget = shrink(histories[0], reproduces)
                            fresh = work / 'fresh-reduced-replay';fresh.mkdir()
                            shutil.copy2(bad, fresh / 'control')
                            path = fresh / 'protocol.json';path.write_text(protocol(reduced))
                            text = run(label+'-fresh-reduced-replay-execute', [fresh / 'control', 'manual', path])
                            try: check(reduced, observations(text))
                            except ValueError as error:
                                if str(error) != failure_signature: raise ValueError('reduced replay changed failure class') from error
                            else: raise ValueError('reduced runtime history failed to reproduce')
                            report['reduction'] = {'status': 'PASS', 'original': histories[0], 'reduced': reduced,
                                'attempts': attempts, 'budget': budget, 'failure_signature': failure_signature,
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
                                      '-Wl,--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t'))}
                for fixture_name, fixture_binary in fixtures.items():
                    text = run(label+'-'+fixture_name+'-execute', [fixture_binary], timeout=90)
                    if fixture_name == 'task-services' and 'task services: all focused checks passed' not in text:
                        raise ValueError('task-service scenario fixture omitted its completion witness')
                    report.setdefault('scenario_cases', []).append({'name': label+'-'+fixture_name,
                        'fixture': fixture_name, 'status': 'PASS', 'sanitizer': sanitizer,
                        'client_optimization': optimization, 'service_optimization': 'O1',
                        'observation_sha256': digest(Path(report['commands'][-1]['log'])),
                        'scenarios': sorted(name for name, owner in report['scenario_inventory'].items()
                                            if owner == fixture_name)})
                    save()
                report.setdefault('sanitizer_policy', {})[label] = policy.finalize()
        expected = len(histories)*2*2*3
        if (len(report['cases']) != expected or len(report['controls']) != 5 or
                len(report.get('scenario_cases', [])) != 2*2*3 or
                {scenario for row in report['scenario_cases'] for scenario in row['scenarios']} !=
                set(report['scenario_inventory'])):
            raise ValueError('runtime schedule configuration/control inventory is incomplete')
        domain, pair_domain = coverage_domain()
        report['model_coverage'] = {'denominator': 'Reviewed 64-seed bounded grammar; not all public API states or implementation coverage.',
            'transition_domain': sorted(domain), 'pair_domain': sorted(map(list, pair_domain)),
            'missing_transitions': sorted(domain-transition_hits),
            'missing_pairs': sorted(map(list, pair_domain-pair_hits))}
        if not replay and (domain-transition_hits or pair_domain-pair_hits):
            raise ValueError('fixed runtime schedule seeds omitted reviewed grammar transitions/pairs')
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
    parser.add_argument('--replay', type=Path)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'cases': len(result['cases'])}))


if __name__ == '__main__': main()
