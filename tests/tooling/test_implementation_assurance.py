"""Exact selection, retained evidence and cleanup controls for implementation analysis."""
import json
from pathlib import Path

import pytest

from scripts.objc3c_implementation_assurance import evidence
from scripts.objc3c_implementation_assurance.plan import select, validate

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


def test_selection_is_scoped_dependency_aware_and_periodically_complete():
    assert not select(['README.md'], **REV)['required']
    selected = select(['native/objc3c/src/runtime/service.cpp', 'README.md'], **REV)
    assert selected['required'] and selected['analysis_paths'] == [
        'native/objc3c/src/runtime/service.cpp']
    assert selected['selection'] == 'dependency-aware-owned-changes'
    periodic = select(['README.md'], **REV, periodic=True)
    assert periodic['required'] and periodic['analysis_paths'] == []
    assert periodic['selection'] == 'complete-owned-tu-inventory'
    assert select([], **REV)['required'] and select(None, **REV)['required']


@pytest.mark.parametrize('mutation', ['required', 'path', 'digest'])
def test_modified_analysis_plan_is_rejected(mutation):
    plan = select(['native/analysis/CMakeLists.txt'], **REV)
    if mutation == 'required': plan['required'] = False
    elif mutation == 'path': plan['analysis_paths'] = []
    else: plan['sha256'] = '0'*64
    with pytest.raises(ValueError):
        validate(plan)


def passing_bundle(tmp_path):
    plan = select(['native/objc3c/src/runtime/service.cpp'], **REV)
    analysis = tmp_path / 'analysis';analysis.mkdir()
    policy = json.loads((Path(__file__).resolve().parents[2] /
                         'toolchains/llvm/implementation-analysis.json').read_text())
    inventory = [{'owner': owner, 'identity': owner, 'dependencies': ['/source/'+owner],
                  'dependency_origin': 'actual-ninja-compilation'} for owner in
                 ('frontend', 'standalone-runtime', 'maintained-clang')]
    results = [{'identity': row['identity'], 'status': 'PASS', 'diagnostics': [],
                'execution_reused': False} for row in inventory]
    controls = [
        {'name': 'control-corrected', 'status': 'PASS', 'exit_code': 0,
         'expected_exit_code': 0, 'timed_out': False},
        {'name': 'control-defective', 'status': 'PASS', 'exit_code': 1,
         'expected_exit_code': 1, 'timed_out': False},
    ]
    inner = {'schema': 'objc3c.implementation-analysis.v1', 'status': 'PASS',
        'subject': 'native-and-maintained-clang-implementation', 'policy': policy,
        'source_build_id': 'fork-build', 'build_config_digest': 'config',
        'tool': {'sha256': 'a'*64}, 'tool_graph': {'libraries': {'libclang.so': 'b'*64}},
        'tool_objects': [{'object': 'tool.o'}], 'inputs': {'policy': 'c'*64},
        'results': results, 'inventory': inventory, 'commands': controls,
        'exclusions': [{'owner': row['owner'], 'selected_variants': 1,
                        'analyzed_variants': 1} for row in inventory],
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'fresh_native_executions': len(results), 'verified_checkpoint_actions': 0}
    (analysis / 'qualification.json').write_text(json.dumps(inner))
    report = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
        'plan_sha256': plan['sha256'], 'analysis_paths': plan['analysis_paths'],
        'temporary_builds_reclaimed': True, 'commands': [],
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'source_build_id': 'fork-build', 'build_config_digest': 'config'}
    report['artifacts'] = [evidence.artifact(tmp_path, path)
                           for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    return plan, report


def test_complete_analysis_bundle_is_admitted(tmp_path):
    plan, _ = passing_bundle(tmp_path)
    result = evidence.aggregate(plan, tmp_path,
        outcomes={'plan': 'success', 'analysis': 'success'}, tested=REV['tested'])
    assert result['status'] == 'PASS' and result['analyzed_variants'] == 3


@pytest.mark.parametrize('fault', ['cleanup', 'artifact', 'control', 'finding', 'outcome'])
def test_incomplete_analysis_bundle_fails_closed(tmp_path, fault):
    plan, report = passing_bundle(tmp_path)
    inner_path = tmp_path / 'analysis/qualification.json'
    inner = json.loads(inner_path.read_text())
    outcomes = {'plan': 'success', 'analysis': 'success'}
    if fault == 'cleanup': report['temporary_builds_reclaimed'] = False
    elif fault == 'artifact': report['artifacts'][0]['sha256'] = '0'*64
    elif fault == 'control': inner['commands'][1]['exit_code'] = 0
    elif fault == 'finding': inner['results'][0]['diagnostics'] = [{'check': 'blocker'}]
    else: outcomes['analysis'] = 'failure'
    inner_path.write_text(json.dumps(inner))
    if fault in ('control', 'finding'):
        report['artifacts'] = [evidence.artifact(tmp_path, path)
                               for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    with pytest.raises(ValueError):
        evidence.aggregate(plan, tmp_path, outcomes=outcomes, tested=REV['tested'])


def test_workflow_has_periodic_full_gate_and_automatic_cleanup():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/implementation-analysis.yml').read_text()
    assert 'schedule:' in workflow and 'workflow_dispatch:' in workflow
    assert workflow.count('--periodic-sha "$GITHUB_SHA"') == 2
    assert 'Linux implementation analysis gate' in workflow
    assert '--retain-builds' not in workflow
    runner = (root / 'scripts/objc3c_implementation_assurance/runner.py').read_text()
    assert "PROFILE = 'compiler-asan-ubsan'" in runner
    assert "('build', 'stage', 'source')" in runner
