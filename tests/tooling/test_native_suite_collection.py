"""Collect independent defects while respecting private SDK consumer dependencies."""
import json
import os
import sys

import pytest

from scripts.objc3c_assurance.execution import checked_command, execute_suites


@pytest.mark.parametrize('failure', [None, 'foundation', 'runtime', 'upstream',
    'package', 'sanitizer-products', 'runtime-schedules', 'interrupt'])
def test_real_native_commands_drain_checks_and_admit_private_sdk_consumers(tmp_path, failure):
    names = ('foundation', 'runtime', 'upstream', 'swift-consumer',
             'semantic-workloads', 'package', 'sanitizer-products', 'runtime-schedules')
    suites = [{'id': name} for name in names]
    report = {'suites': [], 'commands': []};attempted=[];saved=[]
    candidate = tmp_path / 'private-candidate.json'
    def save():
        saved.append(json.loads(json.dumps(report)))
        (tmp_path / 'result.json').write_text(json.dumps(report))
    def execute(suite, directory):
        name = suite['id'];attempted.append(name)
        if failure == 'interrupt' and name == 'upstream':raise KeyboardInterrupt()
        # Exercise an actual file dependency, rather than simulating a consumer
        # that could pass without its sealed package prerequisite.
        code = 'from pathlib import Path\n'
        if name == 'sanitizer-products':
            code += f'assert Path({str(candidate)!r}).read_text() == "sealed candidate"\n'
        code += f'print({name!r})\nraise SystemExit({7 if failure == name else 0})\n'
        checked_command('probe', [sys.executable, '-c', code], repo=tmp_path,
                        environment=dict(os.environ), directory=directory,
                        records=report['commands'])
        if name == 'package':candidate.write_text('sealed candidate')
        return [{'name': name, 'status': 'PASS'}]
    if failure:
        with pytest.raises(KeyboardInterrupt if failure == 'interrupt' else ValueError):
            execute_suites(suites, output=tmp_path, report=report, execute=execute, save=save)
        if failure in ('foundation', 'runtime', 'interrupt'):
            assert attempted[-1] == ('upstream' if failure == 'interrupt' else failure)
            assert not candidate.exists()
        else:
            assert attempted[-1] == 'runtime-schedules'
            rows = {row['id']: row for row in report['suites']}
            failed = rows[failure]
            assert failed['status'] == 'FAIL'
            assert any(row['path'] == failure + '/probe.log' for row in failed['artifacts'])
            assert next(row for row in report['commands'] if row['suite'] == failure)['exit_code'] == 7
            if failure == 'upstream':
                assert rows['package']['status'] == rows['sanitizer-products']['status'] == 'BLOCKED'
                assert 'package' not in attempted and 'sanitizer-products' not in attempted
                assert 'swift-consumer' in attempted and 'semantic-workloads' in attempted
            elif failure == 'package':
                assert rows['sanitizer-products']['status'] == 'BLOCKED'
                assert 'sanitizer-products' not in attempted
            else:
                assert candidate.exists() and rows['package']['status'] == 'PASS'
    else:
        execute_suites(suites, output=tmp_path, report=report, execute=execute, save=save)
        assert attempted == list(names)
        assert all(row['status'] == 'PASS' for row in report['suites'])
    assert saved[-1] == report


@pytest.mark.parametrize('names', [('sanitizer-products',), ('sanitizer-products', 'package')])
def test_missing_or_late_package_is_rejected_before_execution(tmp_path, names):
    def unexpected(*args):raise AssertionError('invalid dependency graph executed a suite')
    with pytest.raises(ValueError, match='earlier package candidate'):
        execute_suites([{'id': name} for name in names], output=tmp_path,
                       report={'suites': []}, execute=unexpected, save=unexpected)
    assert not list(tmp_path.iterdir())
