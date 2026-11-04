"""Defend the independent model and typed reduction's semantic boundaries."""
import copy

import pytest

from scripts.objc3c_toolchain.semantic_model import OPERATIONS, Operation as Op, coverage, generate, interpret, mismatch, transform, validate
from scripts.objc3c_toolchain.semantic_oracles import seed_bank, shrink, source_bundle
from scripts.objc3c_toolchain.semantic_sources import consumer


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
