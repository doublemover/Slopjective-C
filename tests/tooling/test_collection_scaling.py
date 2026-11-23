import json

import pytest

from scripts.objc3c_toolchain.collection_scaling import collection_scaling_result


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
