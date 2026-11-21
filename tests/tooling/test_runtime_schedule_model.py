"""Public-contract histories stay valid and reject clean logical failures."""
import copy
import json

import pytest

from scripts.objc3c_toolchain.runtime_schedule_model import (Model, OPCODES, check, coverage_domain, generate,
    protocol, signature, shrink, validate)
from scripts.objc3c_toolchain.runtime_schedules import observations


@pytest.mark.parametrize('seed', range(64))
def test_generated_histories_are_bounded_reproducible_and_check_every_observation(seed):
    value = generate(seed)
    assert value == generate(seed) and signature(value) == signature(generate(seed))
    trace = Model().run(value['operations'])
    assert check(value, trace)['operations'] == len(value['operations'])
    decoded = json.loads(protocol(value))
    assert [[OPCODES[code], index] for code, index in decoded] == value['operations']
    assert observations('\n'.join(map(json.dumps, trace))) == trace


@pytest.mark.parametrize('mutation', ['version', 'limit', 'missing-create', 'early-release', 'unclosed',
    'no-drain', 'index', 'unknown', 'too-many'])
def test_invalid_histories_are_rejected_before_native_execution(mutation):
    value = generate(0)
    if mutation == 'version': value['model_version'] = 0
    elif mutation == 'limit': value['limits'] = {**value['limits'], 'operations': 10000}
    elif mutation == 'missing-create': value['operations'].remove(['task', 0])
    elif mutation == 'early-release': value['operations'].insert(1, ['release-task', 0])
    elif mutation == 'unclosed': value['operations'] = [op for op in value['operations'] if op[0] != 'close-actor']
    elif mutation == 'no-drain': value['operations'] = [op for op in value['operations'] if op[0] != 'drain']
    elif mutation == 'index': value['operations'][0][1] = 8
    elif mutation == 'unknown': value['operations'][0][0] = 'arbitrary-native-call'
    else: value['operations'] *= 10
    with pytest.raises(ValueError): validate(value)


@pytest.mark.parametrize('mutation', ['destructor', 'callback', 'premature', 'overlap', 'next-twice', 'partial', 'bool'])
def test_semantic_oracle_rejects_wrong_lifetimes_delivery_quiescence_and_consumption(mutation):
    value = generate(0);trace = Model().run(value['operations']);trace = copy.deepcopy(trace)
    index = next(i for i, row in enumerate(trace) if row['operation'] == 'drain')
    if mutation == 'destructor': trace[index]['tasks'][0][1] = 0
    elif mutation == 'callback': trace[-1]['tasks'][0][3] += 1
    elif mutation == 'premature': trace[0]['tasks'][0][1] = 1
    elif mutation == 'overlap': trace[index]['actor_overlap'] = 1
    elif mutation == 'partial': trace.pop()
    elif mutation == 'bool': trace[index]['tasks'][0][1] = True
    else:
        rows = [row for row in trace if row['operation'] == 'next' and row['status'] == 0]
        rows[1]['output'] = rows[0]['output']
    with pytest.raises(ValueError): check(value, trace)


def test_fixed_seeds_cover_the_reviewed_grammar_without_claiming_scheduler_exhaustion():
    transitions, pairs = coverage_domain();observed, adjacent = set(), set()
    for seed in (0, 1, 2, 3, 7, 19, 41, 63):
        model = Model();model.run(generate(seed)['operations'])
        observed.update(model.transitions);adjacent.update(model.pairs)
    assert observed == transitions and adjacent == pairs


def test_shrinking_preserves_public_preconditions_owned_handles_and_explicit_quiescence():
    history = generate(0)
    def defect(candidate):
        validate(candidate)
        return any(op[0] == 'task' for op in candidate['operations'])
    reduced, budget = shrink(history, defect)
    validate(reduced)
    assert defect(reduced) and len(reduced['operations']) < len(history['operations'])
    assert budget['native_trials'] <= 32 and budget['candidate_attempts'] <= 256


@pytest.mark.parametrize('text', ['', '[]', '{}\n'*97, 'not-json'])
def test_missing_malformed_and_excessive_native_ledgers_fail(text):
    with pytest.raises(ValueError): observations(text)
