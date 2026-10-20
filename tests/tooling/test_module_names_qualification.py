import json

import pytest

from scripts.objc3c_toolchain import qualify_module_names as qualification


def row(language, standard, ownership, optimization):
    return {'language': language, 'standard': standard, 'ownership': ownership,
            'optimization': optimization,
            'modules': {'Left': {'semantic_fingerprint': 'a' * 64},
                        'Right': {'semantic_fingerprint': 'b' * 64}},
            'source_executions': 5, 'binary_executions': 1, 'rejections': [{}] * 6}


def test_failure_joins_all_cases_and_replaces_stale_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    output = tmp_path / 'output'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    called = []

    def invoke(sdk, output, *mode):
        called.append(mode)
        if mode == ('objective-c', 'c17', 'arc', 'O0'):
            raise ValueError('forced owned-name failure')
        return row(*mode)

    monkeypatch.setattr(qualification, 'qualify_case', invoke)
    monkeypatch.setattr(qualification, 'qualify_debugger', lambda *a: {'status': 'PASS', 'executions': 6})
    with pytest.raises(ValueError, match='forced owned-name failure'):
        qualification.qualify(sdk, output, jobs=2)
    assert len(called) == 16
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and len(report['cases']) == 15


def test_optimization_cannot_change_either_provider_fingerprint(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()

    def invoke(sdk, output, *mode):
        result = row(*mode)
        result['modules']['Right']['semantic_fingerprint'] = mode[-1]
        return result

    monkeypatch.setattr(qualification, 'qualify_case', invoke)
    monkeypatch.setattr(qualification, 'qualify_debugger', lambda *a: {'status': 'PASS', 'executions': 6})
    with pytest.raises(ValueError, match='optimization'):
        qualification.qualify(sdk, tmp_path / 'output', jobs=2)
    assert json.loads((tmp_path / 'output/qualification.json').read_text())['status'] == 'FAIL'


@pytest.mark.parametrize('jobs', [0, 5, -1])
def test_qualification_preserves_compiler_budget(tmp_path, jobs):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='between 1 and 4'):
        qualification.qualify(sdk, tmp_path / 'output', jobs)
