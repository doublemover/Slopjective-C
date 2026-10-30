"""Fault controls for revision selection, real native execution and terminal gates."""
from copy import deepcopy
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_assurance import evidence
from scripts.objc3c_assurance.execution import checked_command, execute_ctest
from scripts.objc3c_assurance.plan import from_checkout, select, validate

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


@pytest.mark.parametrize('paths', [[], None, ['native/objc3c/src/runtime/x.cpp'],
    ['toolchains/llvm/patches/fix.patch'], ['docs/support/evidence.md'], ['docs/contracts/language.md'],
    ['mystery.new'], ['README.md', 'tests/new.cpp'], ['native/deleted.cpp', 'docs/moved.md']])
def test_nonprose_or_unknown_changes_never_drop_native_execution(paths):
    plan = select(paths, **REV)
    assert {'foundation', 'runtime', 'frontend', 'installed'} <= {suite['id'] for suite in plan['suites']}
    validate(plan)


def test_prose_exclusion_is_explicit_and_periodic_never_uses_it():
    plan = select(['README.md', 'docs/guide.md'], **REV)
    assert plan['classification'] == 'prose-only' and plan['reasons'] and not plan['suites']
    assert len(select(['docs/guide.md'], **REV, periodic=True)['suites']) >= 6
    assert evidence.aggregate(plan, Path('/not-needed'), outcomes={'plan': 'success', 'native': 'skipped'},
                              tested=REV['tested'])['status'] == 'PASS'
    with pytest.raises(ValueError):
        evidence.aggregate(plan, Path('/not-needed'), outcomes={'plan': 'success', 'native': 'success'},
                           tested=REV['tested'])


@pytest.mark.parametrize('paths', [['scripts/objc3c_toolchain/build.py'],
    ['native/objc3c/src/runtime/service.cpp'], ['tests/clang_foundation/codegen.mm'], None])
def test_codegen_and_runtime_changes_select_actual_sanitizer_owner(paths):
    plan = select(paths, **REV)
    assert any(row['id'] == 'sanitizers' and 'sanitizer-detector-control' in row['execution'] for row in plan['suites'])


def test_changed_foundation_cannot_select_the_old_compiler():
    assert select(['toolchains/llvm/cmake/new.cmake'], **REV)['foundation_policy'] == 'rebuild-required'
    assert select(['native/objc3c/src/clang_foundation/x.cpp'], **REV)['foundation_policy'] == 'verify-and-incrementally-build'


@pytest.mark.parametrize('change', ['empty', 'wrong-sha', 'classification', 'digest'])
def test_modified_or_empty_plan_is_not_admitted(change):
    plan = select(['native/x.cpp'], **REV)
    if change == 'empty':
        plan['suites'] = []
    elif change == 'wrong-sha':
        plan['revision']['tested'] = 'd'*40
    elif change == 'classification':
        plan['classification'] = 'prose-only'
    else:
        plan['sha256'] = '0'*64
    with pytest.raises(ValueError):
        validate(plan)


def passing_bundle(tmp_path):
    plan = select(['tests/tooling/contract.py'], **REV)
    rows = []
    for suite in plan['suites']:
        root = tmp_path / suite['id']
        root.mkdir()
        if suite['id'] == 'foundation':
            (root / 'foundation-0.json').write_text(json.dumps({'tests': [{'name': 'parser rejection', 'code': 'PASS'}]}))
            source = {'status': 'passed', 'phases': [dict(phase=name, exit_code=0, **(
                {'test_results': '/producer/foundation-0.json'} if name == 'test' else {}))
                for name in ('configure', 'build', 'test', 'stage')]}
            (root / 'foundation.json').write_text(json.dumps(source))
        elif suite['id'] == 'runtime':
            (root / 'runtime.json').write_text('{"status":"passed"}')
            (root / 'runtime-ctest-inventory.json').write_text('["runtime"]')
            (root / 'runtime-ctest.xml').write_text('<testsuite><testcase name="runtime"/></testsuite>')
        elif suite['id'] == 'frontend':
            (root / 'frontend-tests').mkdir()
            (root / 'frontend-tests/qualification.json').write_text(json.dumps({'status': 'PASS',
                'ctest_test_names': ['objc3_x'], 'frontend_ctests': 1, 'tests': [{'name': 'objc3_x', 'sha256': '0'*64}]}))
            (root / 'frontend-objc3_x-ctest.xml').write_text('<testsuite><testcase name="objc3_x"/></testsuite>')
        else:
            (root / 'installed-tests').mkdir()
            (root / 'installed-tests/summary.json').write_text(json.dumps({'status': 'PASS',
                'commands': [{'name': 'execute', 'exit_code': 0}]}))
        rows.append({'id': suite['id'], 'status': 'PASS', 'executions': evidence.suite_results(tmp_path, suite['id']),
                     'artifacts': [evidence.artifact(tmp_path, p) for p in sorted(root.rglob('*')) if p.is_file()]})
    result = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'suites': rows, 'commands': [{'exit_code': 0}]}
    (tmp_path / 'result.json').write_text(json.dumps(result))
    return plan, result


def verify(selected_plan, tmp_path, **outcomes):
    return evidence.aggregate(selected_plan, tmp_path, outcomes={'plan': 'success', 'native': 'success', **outcomes}, tested=REV['tested'])


def test_complete_bundle_survives_producer_path_relocation(tmp_path):
    plan, _ = passing_bundle(tmp_path)
    assert verify(plan, tmp_path)['status'] == 'PASS'
    assert verify(plan, tmp_path)['suites'] == ['foundation', 'frontend', 'installed', 'runtime']


@pytest.mark.parametrize('fault', ['missing-receipt', 'wrong-sha', 'wrong-plan', 'incomplete', 'failed',
    'missing-suite', 'duplicate-suite', 'no-execution', 'no-artifact', 'missing-artifact', 'changed-artifact',
    'traversal', 'symlink', 'native-cancelled', 'native-skipped', 'native-failed', 'plan-failed',
    'failed-command', 'inner-failure', 'empty-native-results', 'unexpected-skip'])
def test_required_gate_rejects_fault_injections(tmp_path, fault):
    plan, result = passing_bundle(tmp_path)
    outcomes = {}
    if fault == 'missing-receipt':
        (tmp_path / 'result.json').unlink()
    elif fault == 'wrong-sha':
        result['revision'] = {**REV, 'tested': 'd'*40}
    elif fault == 'wrong-plan':
        result['plan_sha256'] = '0'*64
    elif fault in ('incomplete', 'failed'):
        result['status'] = 'running' if fault == 'incomplete' else 'FAIL'
    elif fault == 'missing-suite':
        result['suites'].pop()
    elif fault == 'duplicate-suite':
        result['suites'][-1] = deepcopy(result['suites'][0])
    elif fault == 'no-execution':
        result['suites'][0]['executions'] = []
    elif fault == 'no-artifact':
        result['suites'][0]['artifacts'] = []
    elif fault in ('missing-artifact', 'changed-artifact', 'symlink'):
        path = tmp_path / result['suites'][0]['artifacts'][0]['path']
        path.unlink()
        if fault == 'changed-artifact':
            path.write_text('substituted cached result')
        elif fault == 'symlink':
            path.symlink_to(tmp_path / 'result.json')
    elif fault == 'traversal':
        result['suites'][0]['artifacts'][0]['path'] = '../outside.json'
    elif fault.startswith('native-'):
        outcomes['native'] = fault.removeprefix('native-').replace('failed', 'failure')
    elif fault == 'plan-failed':
        outcomes['plan'] = 'failure'
    elif fault == 'failed-command':
        result['commands'][0]['exit_code'] = 1
    else:
        path = tmp_path / ('runtime/runtime-ctest.xml' if fault == 'unexpected-skip' else 'foundation/foundation-0.json')
        path.write_text('<testsuite><testcase name="runtime"><skipped/></testcase></testsuite>' if fault == 'unexpected-skip'
                        else json.dumps({'tests': [] if fault == 'empty-native-results' else [{'name': 'bad', 'code': 'FAIL'}]}))
        # Even an updated content hash cannot turn an inner failure into a pass.
        suite = next(row for row in result['suites'] if row['id'] == path.parent.name)
        suite['artifacts'] = [evidence.artifact(tmp_path, tmp_path / row['path']) for row in suite['artifacts']]
    if fault != 'missing-receipt':
        (tmp_path / 'result.json').write_text(json.dumps(result))
    with pytest.raises((ValueError, OSError)):
        verify(plan, tmp_path, **outcomes)


def test_actual_git_revision_and_deleted_source_drive_selection(tmp_path):
    def git(*args):
        return subprocess.check_output(['git', '-C', str(tmp_path), *args], text=True).strip()
    git('init', '-q')
    git('config', 'user.name', 'Assurance Test')
    git('config', 'user.email', 'assurance@example.invalid')
    (tmp_path / 'native.cpp').write_text('int x;')
    git('add', '.'); git('commit', '-qm', 'base')
    base = git('rev-parse', 'HEAD')
    (tmp_path / 'native.cpp').unlink()
    (tmp_path / 'README.md').write_text('moved implementation')
    git('add', '-A'); git('commit', '-qm', 'delete code')
    head = git('rev-parse', 'HEAD')
    assert from_checkout(tmp_path, head=head, base=base, tested=head)['classification'] == 'native'
    with pytest.raises(ValueError, match='tested SHA'):
        from_checkout(tmp_path, head=head, base=base, tested=base)
    (tmp_path / 'README.md').write_text('dirty')
    with pytest.raises(ValueError, match='tracked source'):
        from_checkout(tmp_path, head=head, base=base, tested=head)


@pytest.mark.parametrize('fault', ['good', 'compile', 'runtime', 'missing-binary', 'skip', 'empty', 'new-test'])
def test_real_native_commands_and_ctest_fail_closed(tmp_path, fault):
    """Compile and execute real C; no mock may satisfy this boundary control."""
    source = tmp_path / 'src'; source.mkdir()
    build = tmp_path / 'build'
    output = tmp_path / 'evidence'; output.mkdir()
    (source / 'main.c').write_text('this is invalid C' if fault == 'compile' else
                                'int main(void) { return ' + ('1' if fault == 'runtime' else '0') + '; }')
    tests = '' if fault == 'empty' else 'add_test(NAME witness COMMAND witness)\n'
    if fault == 'skip':
        tests += 'set_tests_properties(witness PROPERTIES SKIP_RETURN_CODE 0)\n'
    if fault == 'new-test':
        tests += 'add_test(NAME added_later COMMAND witness)\n'
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(assurance_fault C)\nenable_testing()\nadd_executable(witness main.c)\n' + tests)
    records = []
    def command(label, argv):
        checked_command(label, argv, repo=tmp_path, environment=os.environ.copy(), directory=output, records=records)
    command('configure', ['cmake', '-S', source, '-B', build, '-G', 'Ninja'])
    if fault == 'compile':
        with pytest.raises(ValueError, match='native command failed'):
            command('compile', ['cmake', '--build', build])
        assert records[-1]['exit_code'] != 0 and (output / 'compile.log').is_file()
        return
    command('compile', ['cmake', '--build', build])
    if fault == 'missing-binary':
        (build / 'witness').unlink()
    if fault in ('good', 'new-test'):
        rows = execute_ctest(build, 'actual', output, command)
        assert {row['name'] for row in rows} == ({'witness', 'added_later'} if fault == 'new-test' else {'witness'})
    else:
        with pytest.raises(ValueError):
            execute_ctest(build, 'actual', output, command)
