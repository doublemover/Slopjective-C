import json

import pytest

from scripts.objc3c_toolchain.collection_scaling import (
    collection_scaling_result, expected_collection_phases,
)


@pytest.fixture
def map_phases():
    return {
        'construct': {'completed_operations': 528, 'checksum': 0},
        'lookup': {'completed_operations': 1024, 'checksum': 23808},
        'duplicate': {'completed_operations': 512, 'checksum': 0},
        'iterate': {'completed_operations': 560, 'checksum': 32256},
        'remove': {'completed_operations': 128, 'checksum': 0},
        'teardown': {'completed_operations': 16, 'checksum': 0},
    }


def test_map_phase_oracle_checks_counts_and_checksum(map_phases):
    for phase in map_phases.values():
        phase['nanoseconds'] = 1
    assert collection_scaling_result(json.dumps(map_phases), 'map', 32, 'time') == map_phases
    map_phases['iterate']['checksum'] -= 1
    with pytest.raises(ValueError, match='disagrees with the oracle: iterate'):
        collection_scaling_result(json.dumps(map_phases), 'map', 32, 'time')
    with pytest.raises(ValueError, match='disagrees with the oracle: lookup'):
        collection_scaling_result(json.dumps(map_phases), 'set', 32, 'time')


def test_allocate_requests_are_separate_from_phase_timing(map_phases):
    for phase in map_phases.values():
        phase.update(allocations=0, allocated_bytes=0)
    with pytest.raises(ValueError, match='did not observe construction allocations'):
        collection_scaling_result(json.dumps(map_phases), 'map', 32, 'allocations')
    map_phases['construct'].update(allocations=33, allocated_bytes=528)
    assert collection_scaling_result(json.dumps(map_phases), 'map', 32, 'allocations') == map_phases
    map_phases['lookup']['nanoseconds'] = 1
    with pytest.raises(ValueError, match='disagrees with the oracle: lookup'):
        collection_scaling_result(json.dumps(map_phases), 'map', 32, 'allocations')


def test_retained_index_bytes_retire_through_erase_and_release(map_phases):
    for phase in map_phases.values():
        phase.update(allocations=0, allocated_bytes=0)
    map_phases['construct'].update(allocations=48, allocated_bytes=2520,
                                  index_bytes_before=0, retained_index_bytes=1296)
    map_phases['remove'].update(index_bytes_before=1296, retained_index_bytes=1104)
    map_phases['teardown'].update(index_bytes_before=1104, retained_index_bytes=0)
    assert collection_scaling_result(json.dumps(map_phases), 'map', 32, 'service-diagnostics')
    map_phases['teardown']['retained_index_bytes'] = 1
    with pytest.raises(ValueError, match='contradicted retained index bytes'):
        collection_scaling_result(json.dumps(map_phases), 'map', 32, 'service-diagnostics')


def test_associative_rollback_requires_copy_lifetime_balance():
    phases = expected_collection_phases('generic-map-string', 1, rounds=1)
    for phase in phases.values():
        phase.update(allocations=0, allocated_bytes=0, key_copies=0,
                     value_copies=0, destroys=0, equals=0, peak_payload_bytes=0)
    for shape in ('unique', 'duplicate', 'collision'):
        phases[shape+'-create'].update(allocations=2, allocated_bytes=200, key_copies=1,
                                      value_copies=1, peak_payload_bytes=200)
        phases[shape+'-release'].update(destroys=2, peak_payload_bytes=200)
    phases['rollback'].update(allocations=1, allocated_bytes=100, key_copies=1,
                              destroys=1, peak_payload_bytes=100)
    assert collection_scaling_result(json.dumps(phases), 'generic-map-string', 1, 'allocations', 1)
    phases['rollback']['destroys'] = 0
    with pytest.raises(ValueError, match='disagrees with the oracle: rollback'):
        collection_scaling_result(json.dumps(phases), 'generic-map-string', 1, 'allocations', 1)
    with pytest.raises(ValueError, match='did not balance payload ownership: rollback'):
        collection_scaling_result(json.dumps(phases), 'generic-map-string', 1, 'allocations', 1,
                                  candidate=False)


def test_ascii_capacity_uses_one_scan_for_raw_input_and_none_for_validated_views():
    cap = 1 << 20
    phases = expected_collection_phases('text-ascii', cap, rounds=1)
    for phase in phases.values():
        phase.update(allocations=0, allocated_bytes=0, decoded_bytes=0,
                     decoder_calls=0, copied_bytes=0)
    phases['raw-create'].update(allocations=6, allocated_bytes=3 * cap, decoded_bytes=cap,
                                decoder_calls=1, copied_bytes=cap)
    phases['reject-metadata'].update(decoded_bytes=cap, decoder_calls=1)
    for operation in ('concat', 'concat-self', 'append-text'):
        phases[operation]['copied_bytes'] = cap
    phases['scalar-slice']['copied_bytes'] = cap // 2
    phases['finish']['copied_bytes'] = cap * 2
    assert collection_scaling_result(json.dumps(phases), 'text-ascii', cap, 'service-diagnostics', 1)
    phases['concat']['decoded_bytes'] = cap
    with pytest.raises(ValueError, match='disagrees with the oracle: concat'):
        collection_scaling_result(json.dumps(phases), 'text-ascii', cap, 'service-diagnostics', 1)
