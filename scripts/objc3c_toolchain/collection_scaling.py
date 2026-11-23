"""Optional concrete collection phases for the semantic benchmark runner."""
from __future__ import annotations

import json
from pathlib import Path

from .compiler_instrumentation import loaded_graph
from .linux_host import native_loader

COLLECTION_SCALES = (1, 32, 128, 512, 2048)
COLLECTION_ROUNDS = 16
COLLECTION_FAMILIES = ('map', 'set')
COLLECTION_METRICS = ('time', 'allocations')


def collection_scaling_inputs(repo):
    return [repo / 'tests/benchmarks/collection_scaling.cpp', Path(__file__)]


def expected_collection_phases(family, scale, rounds=COLLECTION_ROUNDS):
    if family not in COLLECTION_FAMILIES or scale not in COLLECTION_SCALES:
        raise ValueError('unknown collection scaling workload')
    keys = scale * (scale - 1) // 2
    operations = {
        'construct': scale + 1, 'lookup': 2 * scale, 'duplicate': scale,
        'iterate': scale + 3, 'remove': max(1, scale // 4), 'teardown': 1,
    }
    checksums = {
        'lookup': 3 * keys if family == 'map' else scale,
        'iterate': 4 * keys + scale if family == 'map' else keys,
    }
    return {phase: {'completed_operations': count * rounds,
                    'checksum': checksums.get(phase, 0) * rounds}
            for phase, count in operations.items()}


def collection_scaling_result(payload, family, scale, metric):
    try:
        result = json.loads(payload)
    except ValueError as error:
        raise ValueError('collection scaling fixture omitted its phase results') from error
    expected = expected_collection_phases(family, scale)
    if metric not in COLLECTION_METRICS or not isinstance(result, dict) or set(result) != set(expected):
        raise ValueError('collection scaling fixture changed its phase inventory')
    fields = {'nanoseconds'} if metric == 'time' else {'allocations', 'allocated_bytes'}
    for phase, values in expected.items():
        row = result[phase]
        if (not isinstance(row, dict) or set(row) != set(values) | fields
                or any(type(value) is not int or value < 0 for value in row.values())
                or any(row[key] != value for key, value in values.items())):
            raise ValueError('collection scaling phase disagrees with the oracle: '+phase)
        if metric == 'time' and not row['nanoseconds']:
            raise ValueError('collection scaling fixture omitted timing: '+phase)
        if metric == 'allocations' and (
                (row['allocations'] == 0) != (row['allocated_bytes'] == 0)
                or row['allocated_bytes'] < row['allocations']):
            raise ValueError('collection scaling fixture contradicted allocation diagnostics: '+phase)
    if metric == 'allocations' and not result['construct']['allocations']:
        raise ValueError('collection scaling fixture did not observe construction allocations')
    return result


def measure_collection_scaling(repo, states, repetitions, run, summarize):
    samples, graphs = [], []
    source_path = collection_scaling_inputs(repo)[0]
    binaries = {}
    for state in states:
        index, prefix, root = (state[key] for key in ('index', 'prefix', 'root'))
        source = root / source_path.name
        source.write_bytes(source_path.read_bytes())
        compiler = prefix / 'bin/clang++'
        for metric in COLLECTION_METRICS:
            binary = root / ('collection-scaling-'+metric)
            binary_flags = (['-DOBJC3_BENCH_COLLECT_ALLOCATIONS', '-Wl,--wrap=_Znwm']
                            if metric == 'allocations' else [])
            label = f'sdk-{index}-collection-scaling-{metric}'
            run(label+'-build', [compiler, '--no-default-config', '-resource-dir',
                state['resource_dir'], '-std=c++20', '-O2', '-I'+str(prefix / 'include'),
                source, prefix / 'lib/libobjc3_runtime_services.a', '-pthread',
                '-fuse-ld=lld', *binary_flags, '-o', binary], state)
            graph = loaded_graph(binary, native_loader(), [],
                run=lambda name, argv: Path(run(label+'-'+name, argv, state)['log']).read_text())
            graphs.append({'sdk_index': index, 'metric': metric, 'loader': graph})
            binaries[index, metric] = binary
    for iteration in range(repetitions + 1):
        ordered = states if iteration % 2 == 0 else list(reversed(states))
        for state in ordered:
            index = state['index']
            for metric in COLLECTION_METRICS:
                for family in COLLECTION_FAMILIES:
                    for scale in COLLECTION_SCALES:
                        label = f'{iteration}-sdk-{index}-collection-scaling-{metric}-{family}-{scale}'
                        row = run(label, [binaries[index, metric], family, str(scale),
                                         str(COLLECTION_ROUNDS)], state)
                        result = collection_scaling_result(
                            Path(row['log']).read_text(), family, scale, metric)
                        samples.append({'sdk_index': index, 'metric': metric, 'family': family,
                            'scale': scale, 'warmup': iteration == 0, 'sample': row['name'],
                            'phases': result})
    summaries = []
    for state in states:
        index = state['index']
        for family in COLLECTION_FAMILIES:
            for scale in COLLECTION_SCALES:
                rows = [row['phases'] for row in samples if row['sdk_index'] == index
                        and row['metric'] == 'time' and row['family'] == family
                        and row['scale'] == scale and not row['warmup']]
                phases = {phase: summarize([{'wall_seconds': row[phase]['nanoseconds'] / 1e9}
                                            for row in rows])
                          for phase in expected_collection_phases(family, scale)}
                summaries.append({'sdk_index': index, 'family': family, 'scale': scale,
                                  'phase_seconds': phases})
    return {'status': 'PASS', 'rounds': COLLECTION_ROUNDS,
            'allocation_scope': 'Unaligned operator new requests counted in a separate executable.',
            'timing_scope': 'Single-threaded phase timings with semantic checks and separate teardown.',
            'samples': samples, 'summaries': summaries, 'loaders': graphs}
