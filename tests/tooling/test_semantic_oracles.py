"""Defend the independent model and typed reduction's semantic boundaries."""
import copy
import json
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.semantic_model import (OPERATIONS, EligibilityFailure, Operation as Op,
    coverage, eligibility, generate, interpret, mismatch, transform, transition_domain, validate)
from scripts.objc3c_toolchain.semantic_oracles import replay, seed_bank, shrink, source_bundle
from scripts.objc3c_toolchain.semantic_sources import consumer
from scripts.objc3c_toolchain.semantic_generation import program as strategy_program


def test_curated_seed_prefix_covers_every_modeled_operation_reproducibly():
    assert generate(7) == generate(7)
    assert generate(7) != generate(19)
    for seed in (7, 19, 41):
        assert {op.kind for op in generate(seed)} == set(OPERATIONS)
        validate(generate(seed))
    transitions = coverage([generate(seed) for seed in (7, 19, 41)])
    assert len(transitions['optional_tag_transitions']) == len(transitions['result_tag_transitions']) == 4
    with pytest.raises(ValueError, match='active-arm transitions'):
        coverage([[Op('none')]])


def test_logical_owner_model_keeps_a_shared_object_until_its_last_owner_leaves():
    result = interpret([Op('new'), Op('copy_object', slot=1, other=0), Op('clear'), Op('clear', slot=1)])
    assert [row[8:] for row in result['states']] == [
        [1, 0, 1, 0], [1, 1, 1, 0], [0, 1, 1, 0], [0, 0, 0, 1], [0, 0, 0, 1]]
    assert result['events'] == []


def test_scalar_success_and_error_transport_are_independent_of_lowering():
    result = interpret([Op('ok', value=4), Op('step', value=2), Op('err', value=-3), Op('step', value=5)])
    assert [row[4:6] for row in result['states']] == [[1, 4], [1, 6], [0, -3], [0, -3], [0, -3]]
    assert interpret([Op('ok', value=4), Op('step', value=2)], bias=1)['states'][1][5] == 7


def test_native_scope_and_partial_constructor_failure_have_defined_cleanup_order():
    assert interpret([Op('scope'), Op('partial_throw')])['events'] == [
        1000, 2000, 3000, 4000, 1001, 2001, 3001, 4001, 5001]


@pytest.mark.parametrize('program', [[], [Op('move-from')], [Op('some', slot=2)],
    [Op('some', value=21)], [Op('some', value=True)], [Op('some', other=False)],
    [Op('new')]*33, [Op('none')]*65, ['unknown']])
def test_unsupported_or_undefined_generator_inputs_fail_before_compilation(program):
    with pytest.raises(ValueError):
        validate(program)


@pytest.mark.parametrize('relation', ['rename', 'comments', 'pure-grouping'])
def test_reviewed_relations_preserve_the_typed_model_without_changing_exported_identities(relation):
    program = generate(7)
    changed = transform(program, relation)
    assert changed == program and changed is not program
    assert interpret(changed) == interpret(program)
    assert consumer(program, relation=relation) != consumer(program)


@pytest.mark.parametrize('relation', ['reverse', 'float-reassociate', 'ownership-elision'])
def test_invalid_metamorphic_transformations_are_generator_errors(relation):
    with pytest.raises(ValueError, match='preconditions'):
        transform(generate(7), relation)


def test_zero_exit_and_differential_agreement_cannot_hide_wrong_semantics():
    expected = interpret([Op('ok', value=12)])
    assert mismatch(expected, copy.deepcopy(expected)) is None
    wrong = copy.deepcopy(expected)
    wrong['states'][0][4] = 0
    assert mismatch(expected, wrong) == 'value-or-lifetime'
    wrong = copy.deepcopy(expected)
    wrong['events'].append(99)
    assert mismatch(expected, wrong) == 'cleanup-order'
    wrong = copy.deepcopy(expected)
    wrong['states'][0][4] = True
    assert mismatch(expected, wrong) == 'output-schema'


def test_typed_shrinking_preserves_the_specific_mismatch_and_rejects_unrelated_failures():
    original = generate(7)
    calls = []
    def predicate(program):
        validate(program)
        calls.append(program)
        return 'value-or-lifetime' if any(op.kind == 'err' for op in program) else 'compiler-crash'
    reduced = shrink(original, predicate, 'value-or-lifetime')
    assert len(reduced) == 1 and reduced[0].kind == 'err'
    assert original == generate(7) and len(calls) > 1


@pytest.mark.parametrize('configuration', [dict(transport='cache-fallback'), dict(optimization='O3'),
    dict(standard='c++17'), dict(relation='reverse'), dict(mutant='unknown'), dict(bias=True),
    dict(bias=20), dict(sanitized=1), dict(expected_mismatch='compiler-crash')])
def test_replay_configuration_rejects_unsupported_routes_before_creating_outputs(tmp_path, configuration):
    output = tmp_path / 'case'
    with pytest.raises(ValueError, match='configuration'):
        source_bundle(generate(7), output, **configuration)
    assert not output.exists()


def test_long_campaign_seeds_are_bounded_reproducible_and_keep_the_reviewed_bank():
    seeds = seed_bank([7, 19, 41], 'a'*40)
    assert seeds[:3] == (7, 19, 41) and len(seeds) == 6
    assert seeds == seed_bank([7, 19, 41], 'a'*40)
    assert seeds != seed_bank([7, 19, 41], 'b'*40)
    for seed in seeds:
        validate(generate(seed))
    with pytest.raises(ValueError, match='tested revision'):
        seed_bank([7], 'HEAD')


@pytest.mark.parametrize('kind', OPERATIONS)
@pytest.mark.parametrize('value', [-2**31, -21, 21, 2**31-1])
def test_integer_limits_and_adjacent_operand_boundaries_are_ineligible(kind, value):
    validate([Op(kind, value=-20), Op(kind, value=20)])
    with pytest.raises(EligibilityFailure, match='out-of-range'):
        eligibility([Op(kind, value=value)])


def test_maximum_length_arithmetic_remains_within_the_declared_int32_envelope():
    program = [Op('ok', value=20), *[Op('step', value=20)]*63]
    for bias in (0, 1):
        manifest = eligibility(program, bias=bias, mutant='provider')
        assert max(row[5] for row in interpret(program, bias=bias)['states']) < manifest['bounds']['signed_intermediate_abs_max']


@pytest.mark.parametrize('change', ['width', 'runtime', 'witness', 'precondition', 'operation',
                                  'version', 'observations', 'missing'])
def test_incompatible_replay_is_an_eligibility_failure_before_any_compiler_runs(tmp_path, change):
    root = tmp_path / 'record'
    source_bundle(generate(7), root, relation='rename')
    record = json.loads((root / 'program.json').read_text())
    if change == 'width': record['eligibility']['profile']['int_bits'] = 64
    elif change == 'runtime': record['eligibility']['profile']['runtime'] = 'ambient'
    elif change == 'witness': record['eligibility']['witness']['argv'] = ['unreviewed']
    elif change == 'precondition': record['eligibility']['relation']['preconditions'] = []
    elif change == 'operation': record['operations'][0]['value'] += 1
    elif change == 'version': record['model_version'] = 1
    elif change == 'observations': record['expected']['events'].append(999)
    elif change == 'missing': del record['eligibility']
    document = root / 'incompatible.json'
    document.write_text(json.dumps(record))
    output = tmp_path / 'must-not-exist'
    with pytest.raises(EligibilityFailure):
        replay(tmp_path, output, document, facade=Path('/missing'), services=Path('/missing'), runtime=Path('/missing'))
    assert not output.exists()


def test_transition_coverage_has_a_stable_declared_domain_and_pair_denominator():
    result = coverage([generate(seed) for seed in (7, 19, 41)])
    assert set(result['transition_ids']) <= transition_domain()
    assert result['transitions_supported'] == len(transition_domain())
    assert result['transitions_covered'] == len(result['transition_ids'])
    assert result['pairs_supported'] == len(OPERATIONS)**2
    assert result['pairs_covered'] == len(result['pair_ids'])
    assert any(name.startswith('ownership:') for name in result['transition_ids'])
    assert 'cleanup:partial_throw' in result['transition_ids']


@pytest.mark.parametrize('kind', ['read-absent', 'move-from', 'reorder-cleanup', 'shift'])
def test_active_arm_moves_cleanup_reordering_and_shifts_need_separate_admission(kind):
    with pytest.raises(EligibilityFailure) as error:
        eligibility([Op(kind)])
    assert error.value.classification == 'unsupported'


@pytest.mark.parametrize('strategy', ['uniform', 'pair-balanced'])
def test_generation_feedback_is_reproducible_bounded_and_separate_from_expectations(strategy):
    for seed in (7, 19, 41):
        witness = strategy_program(seed, strategy)
        assert witness == strategy_program(seed, strategy)
        assert witness[:15] == generate(seed)[:15]
        assert len(witness) == 24
        validate(witness)
    assert strategy_program(7, 'uniform') != strategy_program(7, 'pair-balanced')
    with pytest.raises(ValueError, match='unknown generation'):
        strategy_program(7, 'adaptive')
