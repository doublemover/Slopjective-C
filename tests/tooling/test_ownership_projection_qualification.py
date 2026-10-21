import json

import pytest

from scripts.objc3c_toolchain import qualify_ownership_projections as qualification


def result(mode, optimization):
    return {'language': mode[0], 'standard': mode[1], 'ownership': mode[2],
            'optimization': optimization, 'source_executions': 2, 'binary_executions': 1,
            'rejections': list(range(8)), 'projected_module': {'semantic_fingerprint': 'fixed'}}


def test_failure_joins_all_cases_and_replaces_stale_acceptance(tmp_path, monkeypatch):
    sdk, output = tmp_path / 'sdk', tmp_path / 'output'
    sdk.mkdir()
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    completed = []

    def fail(sdk, output, language, standard, ownership, optimization):
        completed.append((language, standard, ownership, optimization))
        if (language, standard, ownership, optimization) == ('objective-c', 'c17', 'arc', 'O0'):
            raise ValueError('injected broken provider')
        return result((language, standard, ownership), optimization)

    monkeypatch.setattr(qualification, 'qualify_case', fail)
    with pytest.raises(ValueError, match='qualification failed'):
        qualification.qualify(sdk, output, jobs=2)
    report = json.loads((output / 'qualification.json').read_text())
    assert len(completed) == 12
    assert report['status'] == 'FAIL' and len(report['cases']) == 11
    assert report['failures'][0]['error'] == 'injected broken provider'


def test_optimization_cannot_change_projected_semantic_identity(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()

    def unstable(sdk, output, language, standard, ownership, optimization):
        row = result((language, standard, ownership), optimization)
        row['projected_module']['semantic_fingerprint'] = optimization
        return row

    monkeypatch.setattr(qualification, 'qualify_case', unstable)
    with pytest.raises(ValueError, match='qualification failed'):
        qualification.qualify(sdk, tmp_path / 'output', jobs=2)
    report = json.loads((tmp_path / 'output/qualification.json').read_text())
    assert report['status'] == 'FAIL' and len(report['failures']) == 6
    assert len(report['cases']) == 12


@pytest.mark.parametrize('jobs', [0, 5, -1])
def test_qualification_preserves_compiler_worker_budget(tmp_path, jobs):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='between 1 and 4'):
        qualification.qualify(sdk, tmp_path / 'output', jobs=jobs)
    assert not (tmp_path / 'output').exists()
