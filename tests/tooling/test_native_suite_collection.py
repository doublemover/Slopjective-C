"""One compiler batch must collect independent defects without delivering a failed SDK."""
import json
import os
import sys

import pytest

from scripts.objc3c_assurance.execution import checked_command, execute_suites


@pytest.mark.parametrize('failure', [None, 'foundation', 'runtime', 'upstream', 'interrupt'])
def test_real_native_commands_drain_independent_checks_and_guard_delivery(tmp_path, failure):
    suites = [{'id': name} for name in ('foundation','runtime','upstream','package','swift-consumer','semantic-workloads')]
    report = {'suites': [], 'commands': []};attempted=[];saved=[]
    def save():
        saved.append(json.loads(json.dumps(report)))
        (tmp_path/'result.json').write_text(json.dumps(report))
    def execute(suite, directory):
        name = suite['id'];attempted.append(name)
        if failure == 'interrupt' and name == 'upstream':raise KeyboardInterrupt()
        checked_command('probe', [sys.executable,'-c',f'print("{name}");raise SystemExit({7 if failure==name else 0})'],
                        repo=tmp_path,environment=dict(os.environ),directory=directory,records=report['commands'])
        return [{'name':name,'status':'PASS'}]
    if failure:
        with pytest.raises(KeyboardInterrupt if failure=='interrupt' else ValueError):
            execute_suites(suites,output=tmp_path,report=report,execute=execute,save=save)
        assert 'package' not in attempted
        if failure=='upstream':
            assert attempted[-2:]==['swift-consumer','semantic-workloads']
            assert report['suites'][-1]['id']=='package' and report['suites'][-1]['status']=='BLOCKED'
            failed=next(row for row in report['suites'] if row['id']=='upstream')
            assert failed['status']=='FAIL' and failed['artifacts'][0]['path']=='upstream/probe.log'
            assert report['commands'][2]['exit_code']==7
        else:
            assert attempted[-1]==('upstream' if failure=='interrupt' else failure)
    else:
        execute_suites(suites,output=tmp_path,report=report,execute=execute,save=save)
        assert attempted[-1]=='package'
        assert all(row['status']=='PASS' for row in report['suites'])
    assert saved[-1]==report
