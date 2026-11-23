"""Optional collection and text phases for the semantic benchmark runner."""
from __future__ import annotations

import json
from pathlib import Path
import time

from .compiler_instrumentation import loaded_graph
from .linux_host import native_loader

COLLECTION_SCALES = (1, 32, 128, 512, 2048)
COLLECTION_ROUNDS = 16
COLLECTION_FAMILIES = ('map', 'set')
GENERIC_FAMILIES = ('generic-map-int', 'generic-map-string', 'generic-set-int',
                    'generic-set-string', 'array-int', 'array-string')
TEXT_FAMILIES = ('text-ascii', 'text-mixed')
TEXT_SCALES = (1024, 65536, (1 << 20) - 1, 1 << 20)
MANAGED_ROUNDS = 4
COLLECTION_METRICS = ('time', 'allocations', 'service-diagnostics')
DIAGNOSTIC_DEFINITIONS = ('OBJC3_RUNTIME_I32_INDEX_MEMORY_TESTING',
                          'OBJC3_GENERIC_ARRAY_STORAGE_TESTING',
                          'OBJC3_TEXT_CONSTRUCTION_TESTING')
DIAGNOSTIC_UNITS = ('collections_service.cpp', 'generic_collection_service.cpp',
                    'text_service.cpp')
COLLECTION_EVIDENCE_SCOPE = {
    'allocation_scope': 'Ordinary and aligned nothrow operator new requests in a separate executable.',
    'timing_scope': 'Single-threaded SDK archive phase timings with separate teardown.',
    'diagnostic_scope': ('Candidate compiler with three instrumented service units from the checkout. '
        'Index bytes sum the index object and live allocator requests across rounds, '
        'including nodes and buckets, on the measured standard library. '
        'Payload peak bytes include runtime-owned object storage and string buffer capacities.'),
    'timeline_scope': 'Monotonic offsets around each command and its receipt write within one observation.',
}


def collection_scaling_inputs(repo):
    runtime = repo / 'native/objc3c/src/runtime'
    return [repo / 'tests/benchmarks/collection_scaling.cpp',
            repo / 'scripts/objc3c_toolchain/collection_scaling.py',
            *(runtime / 'stdlib' / name for name in DIAGNOSTIC_UNITS),
            *(runtime / directory / name for directory, name in (
                ('stdlib', 'opaque_service_storage.h'), ('stdlib', 'stdlib_service_contract.h'),
                ('stdlib', 'text_utf8_decode.h'), ('public', 'generic_collection_service.h'),
                ('public', 'objc3_runtime_services.h'), ('public', 'objc3_text.h'),
                ('public', 'objc3_collections.h'), ('public', 'objc3_stdlib_status.h')))]


def collection_cases():
    return [(family, scale, rounds)
            for families, scales, rounds in (
                (COLLECTION_FAMILIES, COLLECTION_SCALES, COLLECTION_ROUNDS),
                (GENERIC_FAMILIES, COLLECTION_SCALES, MANAGED_ROUNDS),
                (TEXT_FAMILIES, TEXT_SCALES, MANAGED_ROUNDS))
            for family in families for scale in scales]


def collection_sample_cases(metric):
    return [case for case in collection_cases()
            if metric != 'service-diagnostics' or not case[0].startswith('generic-')]


def collection_phase_timing(family, scale, phases):
    timing = {phase: phases[phase]['nanoseconds'] / 1e9
              for phase in expected_collection_phases(family, scale)}
    if family.startswith('generic-'):
        for shape in _associative_shapes(scale):
            timing[shape+'-create-and-release'] = (
                phases[shape+'-create']['nanoseconds'] + phases[shape+'-release']['nanoseconds']) / 1e9
    return timing


def _concrete_work(family, scale):
    keys = scale * (scale - 1) // 2
    operations = {
        'construct': scale + 1, 'lookup': 2 * scale, 'duplicate': scale,
        'iterate': scale + 3, 'remove': max(1, scale // 4), 'teardown': 1,
    }
    checksums = {
        'lookup': 3 * keys if family == 'map' else scale,
        'iterate': 4 * keys + scale if family == 'map' else keys,
    }
    return operations, checksums


def _associative_shapes(scale):
    return {'unique': scale, 'duplicate': (scale + 3) // 4, 'collision': scale}


def _associative_work(family, scale):
    operations, checksums = {}, {}
    for shape, distinct in _associative_shapes(scale).items():
        operations[shape+'-create'] = scale + 1
        operations[shape+'-release'] = 1
        checksums[shape+'-create'] = sum(range(distinct))
        if family.startswith('generic-map-'):
            checksums[shape+'-create'] += sum(
                3 * (key + (scale - 1 - key) // distinct * distinct) + 1
                for key in range(distinct))
    operations['rollback'] = scale // 2 + 1
    return operations, checksums


def _array_work(scale):
    writes = max(1, scale // 4)
    keys = scale * (scale - 1) // 2
    after_replace = keys - scale // 2 + scale + writes - 1
    return ({'append': scale + 1, 'replace': writes, 'get': scale, 'iterate': scale + 3,
             'release': 1, 'bulk': scale + 1, 'bulk-get': scale, 'bulk-iterate': scale + 3,
             'bulk-release': 1},
            {'get': after_replace, 'iterate': after_replace, 'bulk-get': keys, 'bulk-iterate': keys})


def _text_work(family, size):
    mixed = family == 'text-mixed'
    pattern = (0, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0x9f, 0x99, 0x82) if mixed else (0, 65, 66, 67)
    groups, tail = divmod(size, len(pattern))
    scalars = groups * 4 + tail

    def boundary(index):
        if not mixed:
            return index
        if index >= groups * 4:
            return groups * 10 + index - groups * 4
        return index // 4 * 10 + (0, 1, 3, 6)[index % 4]

    def checksum(start, length, count, repeat=False):
        positions = (0, length // 4, length // 2, 3 * length // 4, length - 1)
        offsets = (start + (position % (length // 2) if repeat else position)
                   for position in positions)
        return length + count + sum(pattern[index % len(pattern)] if index < groups * len(pattern)
                                    else 120 for index in offsets)

    half = scalars // 2
    left_scalars = size // 20 * 4 if mixed else half
    split = boundary(left_scalars)
    start, end = boundary(scalars // 4), boundary(scalars // 4 + half)
    full_checksum = checksum(0, size, scalars)
    operations = {'raw-create': 1, 'concat': 1, 'concat-self': 1, 'scalar-slice': 1,
                  'append-text': 1, 'finish': 2, 'release': 7, 'reject-metadata': 1}
    checksums = {'raw-create': full_checksum, 'concat': full_checksum, 'finish': 2 * full_checksum,
                 'concat-self': checksum(0, 2 * split, 2 * left_scalars, repeat=True),
                 'scalar-slice': checksum(start, end - start, half)}
    return operations, checksums, {'raw-create': size, 'concat': size, 'concat-self': 2 * split,
                                   'scalar-slice': end - start, 'append-text': size, 'finish': 2 * size}


def expected_collection_phases(family, scale, rounds=None):
    if family in COLLECTION_FAMILIES and scale in COLLECTION_SCALES:
        operations, checksums = _concrete_work(family, scale)
        default_rounds = COLLECTION_ROUNDS
    elif family in GENERIC_FAMILIES and scale in COLLECTION_SCALES:
        operations, checksums = (_array_work(scale) if family.startswith('array-')
                                 else _associative_work(family, scale))
        default_rounds = MANAGED_ROUNDS
    elif family in TEXT_FAMILIES and scale in TEXT_SCALES:
        operations, checksums, _ = _text_work(family, scale)
        default_rounds = MANAGED_ROUNDS
    else:
        raise ValueError('unknown collection scaling workload')
    if rounds is None:
        rounds = default_rounds
    return {phase: {'completed_operations': count * rounds,
                    'checksum': checksums.get(phase, 0) * rounds}
            for phase, count in operations.items()}


def _expected_ownership(family, scale, phase):
    if family.startswith('array-'):
        copies, destroys = {
            'append': (sum(index % 32 + 1 for index in range(scale)), 0),
            'replace': (max(1, scale // 4) * min(32, scale - scale // 2 // 32 * 32), 0),
            'get': (scale, scale), 'iterate': (scale, scale),
            'bulk-get': (scale, scale), 'bulk-iterate': (scale, scale),
            'release': (0, scale), 'bulk-release': (0, scale),
            'bulk': (scale, 0),
        }[phase]
        if phase == 'append':
            destroys = copies - scale
        if phase == 'replace':
            destroys = copies
        return {'key_copies': 0, 'value_copies': copies, 'destroys': destroys, 'equals': 0}
    mapped = family.startswith('generic-map-')
    if phase == 'rollback':
        keys = scale // 2 + int(mapped)
        values = scale // 2 if mapped else 0
        equals = scale // 2 * (scale // 2 + 1) // 2
        return {'key_copies': keys, 'value_copies': values, 'destroys': keys + values, 'equals': equals}
    shape, operation = phase.split('-')
    distinct = _associative_shapes(scale)[shape]
    if operation == 'release':
        return {'key_copies': 0, 'value_copies': 0, 'destroys': distinct * (2 if mapped else 1), 'equals': 0}
    equals = distinct * (distinct - 1) // 2 + sum(index % distinct + 1 for index in range(distinct, scale))
    return {'key_copies': distinct, 'value_copies': scale if mapped else 0,
            'destroys': scale - distinct if mapped else 0, 'equals': equals}


def collection_scaling_result(payload, family, scale, metric, rounds=None, *, candidate=True):
    try:
        result = json.loads(payload)
    except ValueError as error:
        raise ValueError('collection scaling fixture omitted its phase results') from error
    expected = expected_collection_phases(family, scale, rounds)
    rounds = rounds or (COLLECTION_ROUNDS if family in COLLECTION_FAMILIES else MANAGED_ROUNDS)
    if metric not in COLLECTION_METRICS or not isinstance(result, dict) or set(result) != set(expected):
        raise ValueError('collection scaling fixture changed its phase inventory')
    allocation = metric != 'time'
    detailed = metric == 'service-diagnostics'
    for phase, values in expected.items():
        row = result[phase]
        fields = {'allocations', 'allocated_bytes'} if allocation else {'nanoseconds'}
        if allocation and family in GENERIC_FAMILIES:
            ownership = _expected_ownership(family, scale, phase)
            if candidate:
                values.update({key: value * rounds for key, value in ownership.items()})
            else:
                fields.update(ownership)
            fields.add('peak_payload_bytes')
        if detailed and family in COLLECTION_FAMILIES and phase in ('construct', 'remove', 'teardown'):
            fields |= {'index_bytes_before', 'retained_index_bytes'}
        if detailed and family.startswith('array-'):
            fields |= {'metadata_nodes', 'metadata_slots'}
        if detailed and family in TEXT_FAMILIES:
            _, _, copied = _text_work(family, scale)
            scans = rounds if phase in ('raw-create', 'reject-metadata') else 0
            values.update(decoder_calls=scans, decoded_bytes=scans * scale,
                          copied_bytes=copied.get(phase, 0) * rounds)
        if (not isinstance(row, dict) or set(row) != set(values) | fields
                or any(type(value) is not int or value < 0 for value in row.values())
                or any(row[key] != value for key, value in values.items())):
            raise ValueError('collection scaling phase disagrees with the oracle: '+phase)
        if metric == 'time' and not row['nanoseconds']:
            raise ValueError('collection scaling fixture omitted timing: '+phase)
        if allocation and (
                (row['allocations'] == 0) != (row['allocated_bytes'] == 0)
                or row['allocated_bytes'] < row['allocations']):
            raise ValueError('collection scaling fixture contradicted allocation diagnostics: '+phase)
        if allocation and family in GENERIC_FAMILIES and (
                bool(row['key_copies'] + row['value_copies'] + row['destroys']) != bool(row['peak_payload_bytes'])):
            raise ValueError('collection scaling fixture contradicted payload lifetime diagnostics: '+phase)
        if allocation and family in GENERIC_FAMILIES:
            balance = sum(ownership[key] for key in ('key_copies', 'value_copies')) - ownership['destroys']
            if row['key_copies'] + row['value_copies'] - row['destroys'] != balance * rounds:
                raise ValueError('collection scaling fixture did not balance payload ownership: '+phase)
        if detailed and family.startswith('array-') and phase in ('append', 'replace'):
            mutations = rounds * (scale if phase == 'append' else max(1, scale // 4))
            if not (0 < row['metadata_nodes'] <= 3 * mutations and row['metadata_slots'] <= 192 * mutations):
                raise ValueError('collection scaling fixture exceeded array metadata bounds: '+phase)
    if allocation and not result[next(iter(expected))]['allocations']:
        raise ValueError('collection scaling fixture did not observe construction allocations')
    if detailed and family in COLLECTION_FAMILIES:
        full, removed, retired = (result[phase] for phase in ('construct', 'remove', 'teardown'))
        if (full['index_bytes_before'] or full['retained_index_bytes'] != removed['index_bytes_before']
                or removed['retained_index_bytes'] != retired['index_bytes_before']
                or not 0 < removed['retained_index_bytes'] < full['retained_index_bytes']
                or retired['retained_index_bytes']):
            raise ValueError('collection scaling fixture contradicted retained index bytes')
    return result


def measure_collection_scaling(repo, states, repetitions, run, summarize):
    samples, graphs = [], []
    source_path = collection_scaling_inputs(repo)[0]
    runtime = repo / 'native/objc3c/src/runtime'
    diagnostic_sources = [runtime / 'stdlib' / name for name in DIAGNOSTIC_UNITS]
    cases = collection_cases()
    binaries = {}
    for state in states:
        index, prefix, root = (state[key] for key in ('index', 'prefix', 'root'))
        source = root / source_path.name
        source.write_bytes(source_path.read_bytes())
        compiler = prefix / 'bin/clang++'
        for metric in COLLECTION_METRICS:
            detailed = metric == 'service-diagnostics'
            if detailed and state is not states[-1]:
                continue
            binary = root / ('collection-scaling-'+metric)
            binary_flags = (['-DOBJC3_BENCH_COLLECT_ALLOCATIONS', '-Wl,--wrap=_Znwm',
                             '-Wl,--wrap=_ZnwmSt11align_val_tRKSt9nothrow_t']
                            if metric != 'time' else [])
            if detailed:
                binary_flags += [
                    '-DOBJC3_BENCH_SERVICE_DIAGNOSTICS', '-I'+str(runtime.parent),
                    *('-D'+define for define in DIAGNOSTIC_DEFINITIONS), *diagnostic_sources]
            label = f'sdk-{index}-collection-scaling-{metric}'
            run(label+'-build', [compiler, '--no-default-config', '-resource-dir',
                state['resource_dir'], '-std=c++20', '-O2', '-I'+str(prefix / 'include'),
                '-ffile-prefix-map='+str(repo)+'=.', '-ffile-prefix-map='+str(root)+'=.',
                source, *binary_flags, prefix / 'lib/libobjc3_runtime_services.a', '-pthread',
                '-fuse-ld=lld', '-Wl,--threads=1', '-o', binary], state)
            graph = loaded_graph(binary, native_loader(), [],
                run=lambda name, argv: Path(run(label+'-'+name, argv, state)['log']).read_text())
            graphs.append({'sdk_index': index, 'metric': metric, 'loader': graph})
            binaries[index, metric] = binary
    origin_ns = time.monotonic_ns()
    for iteration in range(repetitions + 1):
        ordered = states if iteration % 2 == 0 else list(reversed(states))
        for state in ordered:
            index = state['index']
            for metric in COLLECTION_METRICS:
                binary = binaries.get((index, metric))
                if binary is None:
                    continue
                for family, scale, rounds in collection_sample_cases(metric):
                    label = f'{iteration}-sdk-{index}-collection-scaling-{metric}-{family}-{scale}'
                    start_ns = time.monotonic_ns() - origin_ns
                    row = run(label, [binary, family, str(scale), str(rounds)], state)
                    end_ns = time.monotonic_ns() - origin_ns
                    result = collection_scaling_result(Path(row['log']).read_text(),
                        family, scale, metric, rounds, candidate=state is states[-1])
                    samples.append({'sdk_index': index, 'metric': metric, 'family': family,
                        'scale': scale, 'rounds': rounds, 'warmup': iteration == 0,
                        'sample': row['name'], 'phases': result, 'start_ns': start_ns, 'end_ns': end_ns})
    summaries = []
    for state in states:
        index = state['index']
        for family, scale, rounds in cases:
            rows = [collection_phase_timing(family, scale, row['phases'])
                    for row in samples if row['sdk_index'] == index
                    and row['metric'] == 'time' and row['family'] == family
                    and row['scale'] == scale and not row['warmup']]
            phases = {phase: summarize(rows, phase) for phase in rows[0]}
            summaries.append({'sdk_index': index, 'family': family, 'scale': scale,
                              'rounds': rounds, 'phase_seconds': phases})
    return {'status': 'PASS', **COLLECTION_EVIDENCE_SCOPE,
            'samples': samples, 'summaries': summaries, 'loaders': graphs}
