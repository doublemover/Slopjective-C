"""Keep adaptive generation independent of expectations and actual compiler profiles."""
import copy
import gzip
import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import compiler_source_coverage as compiler
from scripts.objc3c_toolchain import semantic_source_feedback as feedback
from scripts.objc3c_toolchain.binary_packages import digest
from scripts.objc3c_toolchain.semantic_model import VERSION, coverage, eligibility, generate


def export(source, *, segments=None, duplicate=False, foreign=False):
    row = {'filename': str(source / ('foreign.cpp' if foreign else 'clang/lib/Sema/ObjC3.cpp')),
           'segments': segments if segments is not None else [
               [1, 1, 0, True, True, False], [2, 1, 4, True, True, False],
               [3, 1, 2, True, True, True], [4, 1, 5, False, False, False]]}
    return json.dumps({'type': 'llvm.coverage.json.export', 'data': [{'files': [row, row] if duplicate else [row]}]})


def test_feedback_uses_actual_owned_source_entry_points_not_gaps_or_model_counts(tmp_path):
    eligible = {'clang/lib/Sema/ObjC3.cpp': 'digest'}
    mapped, hits = feedback.source_points(export(tmp_path), tmp_path, eligible)
    assert mapped == {('clang/lib/Sema/ObjC3.cpp', 1, 1), ('clang/lib/Sema/ObjC3.cpp', 2, 1)}
    assert hits == {('clang/lib/Sema/ObjC3.cpp', 2, 1)}
    for configuration in ({'foreign': True}, {'duplicate': True}, {'segments': [[1, 1, 0, True, True]]}):
        with pytest.raises(ValueError):
            feedback.source_points(export(tmp_path, **configuration), tmp_path, eligible)


@pytest.mark.parametrize('segment', [[1, 1, -1, True, True], [0, 1, 1, True, True],
    [1, 0, 1, True, True], [1, 1, True, True, True], [1, 1, 1, 1, True],
    [1, 1, 1, True, True, 0], [1, 1]])
def test_malformed_profile_segments_cannot_become_positive_feedback(tmp_path, segment):
    with pytest.raises(ValueError, match='segment'):
        feedback.source_points(export(tmp_path, segments=[segment]), tmp_path, {'clang/lib/Sema/ObjC3.cpp': 'digest'})


def test_source_feedback_mapping_has_a_real_bound(tmp_path, monkeypatch):
    monkeypatch.setattr(feedback, 'MAX_POINTS', 1)
    with pytest.raises(ValueError, match='bound'):
        feedback.source_points(export(tmp_path), tmp_path, {'clang/lib/Sema/ObjC3.cpp': 'digest'})


def test_generation_is_reproducible_from_seed_and_feedback_and_preserves_eligibility():
    for seed in (7, 19, 41):
        left, right = feedback.Generator(seed), feedback.Generator(seed)
        first = generate(seed)
        for turn in range(16):
            witness = left.propose()
            assert witness == right.propose()
            assert witness[:15] == first[:15]
            eligibility(witness)
            assert len(witness) == 24
            points = {('unit', turn % 3, 1)}
            assert left.observe(witness, points) == right.observe(witness, points)
            assert len(left.archive) <= 8
        # Semantic operation/transition coverage still has its own denominator.
        assert coverage([list(program) for program, _ in left.archive])


def test_bootstrap_coverage_does_not_permanently_dominate_new_target_discoveries():
    generator = feedback.Generator(7)
    original = generator.propose()
    assert generator.observe(original, set(range(100))) == 100
    assert generator.archive[0][1] == 0
    novel = generator.propose()
    assert generator.observe(novel, set(range(101))) == 1
    assert generator.archive[0] == (tuple(novel), 1)
    generator.observe(generator.propose(), set(range(101)))
    assert generator.archive[0][0] == tuple(novel)


def test_indexed_hit_artifacts_are_small_deterministic_and_replayable(tmp_path):
    points = [('clang/lib/Sema/ObjC3.cpp', index, 1) for index in range(1000)]
    first, second = tmp_path / 'a.gz', tmp_path / 'b.gz'
    a, b = feedback.indexed(first, points), feedback.indexed(second, points)
    assert a['sha256'] == b['sha256']
    assert first.stat().st_size < len(json.dumps(points))//4
    assert json.loads(gzip.decompress(first.read_bytes())) == [list(point) for point in points]


def baseline(tmp_path, monkeypatch):
    services, runtime = tmp_path / 'services.a', tmp_path / 'runtime.so'
    services.write_bytes(b'services');runtime.write_bytes(b'runtime')
    plan = {'source_build_id': 'current-sanitized', 'build_config_digest': 'current-config', 'source_tree': 'current-tree'}
    monkeypatch.setattr(feedback, 'build_plans', lambda *args, **kwargs: {'configure': plan})
    controls = [{'name': 'control-'+name, 'status': 'PASS',
                 'mismatch': 'cleanup-order' if name in ('defer', 'destructor') else 'value-or-lifetime'}
                for name in ('tag', 'defer', 'destructor', 'provider', 'error')]
    inputs = {str(path): digest(path) for path in (services, runtime, *(Path(feedback.__file__).with_name(name)
              for name in ('semantic_model.py', 'semantic_sources.py', 'semantic_oracles.py')))}
    prior = {'schema': 'objc3c.generated-semantic-oracles.v2', 'status': 'PASS', 'model_version': VERSION,
        **plan, 'inputs': inputs, 'controls': controls, 'expected_cases': ['clean'],
        'cases': [{'name': 'clean', 'status': 'PASS', 'mismatch': None}]}
    return prior, services, runtime


def test_source_feedback_requires_the_matching_full_model_baseline(tmp_path, monkeypatch):
    prior, services, runtime = baseline(tmp_path, monkeypatch)
    assert feedback.admit_baseline(tmp_path, prior, services, runtime)['source_tree'] == 'current-tree'
    for key in ('status', 'source_build_id', 'build_config_digest', 'source_tree', 'model_version', 'schema'):
        changed = copy.deepcopy(prior);changed[key] = 'wrong'
        with pytest.raises(ValueError, match='baseline'):
            feedback.admit_baseline(tmp_path, changed, services, runtime)
    for key in ('controls', 'expected_cases', 'cases'):
        changed = copy.deepcopy(prior);changed[key] = []
        with pytest.raises(ValueError, match='baseline'):
            feedback.admit_baseline(tmp_path, changed, services, runtime)
    for key in ('controls', 'expected_cases', 'cases'):
        changed = copy.deepcopy(prior);changed[key].append(changed[key][0])
        with pytest.raises(ValueError, match='baseline'):
            feedback.admit_baseline(tmp_path, changed, services, runtime)
    services.write_bytes(b'replaced runtime input')
    with pytest.raises(ValueError, match='input changed'):
        feedback.admit_baseline(tmp_path, prior, services, runtime)


def test_existing_coverage_product_requires_exact_source_object_and_binary_identity():
    owner = SimpleNamespace(plan={'source_build_id': 'current', 'build_config_digest': 'config',
                                  'instrumentation': {'test_binaries': ['tools/NativeTests']}},
        eligible={'clang/lib/AST/Types.cpp': 'source-bytes'},
        objects=[{'object': 'Types.o', 'sha256': 'object-bytes'}], owner_id='shared-clang-build-id')
    revision = 'a'*40
    prior = {'schema': compiler.SCHEMA, 'status': 'PASS', 'profile': compiler.PROFILE, 'revision': revision,
        'source_build_id': 'current', 'build_config_digest': 'config',
        'coverage_owner': 'shared-clang-cpp-patched-implementation',
        'suites': [{'name': name, 'status': 'PASS'} for name in ('ObjC3-lit', 'NativeTests')],
        'eligible': owner.eligible, 'objects': owner.objects,
        'source_coverage': {'clang/lib/AST/Types.cpp': {'state': 'executed'}},
        'covered_binary_ids': {'/sdk/lib/libclang-cpp.so': owner.owner_id}}
    feedback.admit_coverage_qualification(prior, owner, revision)
    for key in ('status', 'revision', 'source_build_id', 'objects', 'suites', 'covered_binary_ids', 'source_coverage'):
        changed = {**prior, key: {}}
        with pytest.raises(ValueError, match='current compiler source coverage qualification'):
            feedback.admit_coverage_qualification(changed, owner, revision)


def test_measure_uses_no_internal_deadline(tmp_path, monkeypatch):
    runtime = tmp_path / 'runtime.so'
    runtime.touch()
    qualification = tmp_path / 'coverage.json'
    qualification.write_text('{}')
    monkeypatch.setattr(feedback.subprocess, 'check_output', lambda *args, **kwargs: 'a'*40)

    def campaign(*args, **kwargs):
        assert kwargs == {'compiler_profile': feedback.PROFILE, 'command_timeout': None}
        raise ValueError('intentional setup failure')

    monkeypatch.setattr(feedback, 'Campaign', campaign)
    output = tmp_path / 'out/feedback'
    with pytest.raises(ValueError, match='intentional setup failure'):
        feedback.measure(tmp_path, output, runtime=runtime, services=runtime,
                         coverage_qualification=qualification)
    assert json.loads((output / 'qualification.json').read_bytes())['status'] == 'FAIL'


def test_pair_union_requires_every_seed_and_both_strategies_once():
    rows = [{'seed': seed, 'strategy': strategy, 'covered_points': 100 if strategy == 'uniform' else 110,
             'eligible_programs_per_second': 2, 'cpu_seconds': 3}
            for seed in (7, 19, 41) for strategy in feedback.STRATEGIES]
    result = feedback.paired_summary(rows, (7, 19, 41))
    assert result['median_covered_point_ratio'] == 1.1
    assert result['default_strategy'] == 'uniform'
    assert all(row['covered_point_difference'] == 10 for row in result['pairs'])
    for invalid in (rows[:-1], rows+[rows[0]], [*rows[:-1], rows[0]]):
        with pytest.raises(ValueError, match='incomplete or duplicated'):
            feedback.paired_summary(invalid, (7, 19, 41))


def test_foreign_profile_identity_fails_before_merge_and_is_reclaimed(tmp_path):
    directory = tmp_path / 'profiles';directory.mkdir()
    (directory / ('f'*40+'-lane.profraw')).write_bytes(b'not-an-admitted-profile')
    owner = feedback.Feedback.__new__(feedback.Feedback)
    owner.owner_id = 'a'*40;owner.identities = {'clang-cpp': 'a'*40}
    owner.campaign = SimpleNamespace(executor=SimpleNamespace(environment={}))
    with pytest.raises(ValueError, match='binary identities'):
        owner.collect('foreign', directory)
    assert not directory.exists()


def test_case_profiles_use_one_owned_pool_without_a_tool_deadline(tmp_path, monkeypatch):
    owner = feedback.Feedback.__new__(feedback.Feedback)
    owner.root = owner.source = owner.build = tmp_path
    owner.owner, owner.owner_id, owner.mapping = tmp_path / 'libclang-cpp.so', 'a'*40, None
    owner.eligible = {'clang/lib/Sema/ObjC3.cpp': 'digest'}
    owner.identities = {str(owner.owner): owner.owner_id}
    owner.campaign = SimpleNamespace(executor=SimpleNamespace(environment={}), output=tmp_path,
        tools={tool: tool for tool in ('llvm-profdata', 'llvm-cov')},
        report={'compiler_source_coverage': {}, 'commands': []})
    calls = {}

    def run(name, args, *, timeout=None):
        calls[name] = timeout
        if name.endswith('-merge'):
            Path(args[-1]).write_bytes(b'indexed profile')
            return ''
        if name.endswith('-profile-ids'):
            return owner.owner_id
        log = tmp_path / 'export.log'
        log.write_text(export(tmp_path))
        owner.campaign.report['commands'].append({'name': name, 'log': str(log), 'log_sha256': digest(log)})
        return log.read_text()

    owner.campaign.run = run
    monkeypatch.setattr(feedback, 'binary_ids', lambda text: {text})
    directory = owner.begin('case')
    pattern = str(directory / '%b-%m.profraw')
    assert owner.campaign.executor.environment['LLVM_PROFILE_FILE'] == pattern
    (directory / (owner.owner_id+'-identifier_0.profraw')).write_bytes(b'pooled profile')
    points, receipt = owner.collect('case', directory)
    assert points == {('clang/lib/Sema/ObjC3.cpp', 2, 1)}
    assert receipt['profile_pattern'] == pattern and receipt['raw_profiles']['count'] == 1
    assert calls == {'case-merge': None, 'case-profile-ids': None, 'case-export': None}
    assert not directory.exists()


def test_completed_coverage_tool_selection_checks_all_followup_commands(tmp_path, monkeypatch):
    build, stage = tmp_path / 'build', tmp_path / 'stage'
    (build / 'bin').mkdir(parents=True);(build / 'lib').mkdir();(stage / 'lib').mkdir(parents=True)
    for name in ('clang', 'clang++', 'ld.lld', 'llvm-cov', 'llvm-profdata', 'llvm-readobj'):
        (build / 'bin' / name).write_bytes(b'owned tool')
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        (stage / 'lib' / name).write_bytes(b'owned library')
    plans = {phase: {'identity': phase+'-plan', 'command': ['tool'],
                    'follow_up_commands': [['tool-again']] if phase == 'test' else []}
             for phase in ('configure', 'build', 'test', 'stage')}
    plans['configure'].update(paths={'build': str(build), 'stage': str(stage)}, source_build_id='current',
        host_environment={'bootstrap_root': str(tmp_path / 'bootstrap')})
    monkeypatch.setattr(compiler, 'build_plans', lambda *args, **kwargs: plans)
    from scripts.objc3c_toolchain import build as build_owner, llvm_tools
    checked = []
    monkeypatch.setattr(build_owner, 'verify_effective_configuration', lambda plan: checked.append('configuration'))
    monkeypatch.setattr(llvm_tools, 'verify_installation', lambda *args: checked.append('installation'))
    rows = [{'phase': phase, 'plan_identity': phase+'-plan', 'exit_code': 0}
            for phase in ('configure', 'build', 'test', 'test', 'stage')]
    receipt = build / 'foundation-build-report.json'
    receipt.write_text(json.dumps({'status': 'passed', 'source_identity': 'current', 'phases': rows}))
    _, tools, environment = compiler.selected_tools(tmp_path)
    assert 'llvm-cov' in tools and environment['LLVM_PROFILE_FILE'] == '/dev/null'
    assert checked.count('configuration') == 1 and checked.count('installation') == 8
    receipt.write_text(json.dumps({'status': 'passed', 'source_identity': 'current',
        'phases': [{**row, 'plan_identity': 'historical-pre-stage-view'} for row in rows]}))
    compiler.selected_tools(tmp_path)
    for changed in (rows[:-2]+rows[-1:], rows+[rows[2]], [*rows[:-1], {**rows[-1], 'exit_code': 1}]):
        receipt.write_text(json.dumps({'status': 'passed', 'source_identity': 'current', 'phases': changed}))
        with pytest.raises(ValueError, match='complete matching'):
            compiler.selected_tools(tmp_path)
