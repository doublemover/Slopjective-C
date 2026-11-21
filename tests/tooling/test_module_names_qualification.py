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
    monkeypatch.setattr(qualification, 'qualify_collision_debugger', lambda *a: {'status': 'PASS', 'executions': 12})
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
    monkeypatch.setattr(qualification, 'qualify_collision_debugger', lambda *a: {'status': 'PASS', 'executions': 12})
    with pytest.raises(ValueError, match='optimization'):
        qualification.qualify(sdk, tmp_path / 'output', jobs=2)
    assert json.loads((tmp_path / 'output/qualification.json').read_text())['status'] == 'FAIL'


@pytest.mark.parametrize('jobs', [0, 5, -1])
def test_qualification_preserves_compiler_budget(tmp_path, jobs):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='between 1 and 4'):
        qualification.qualify(sdk, tmp_path / 'output', jobs)


MODULE = {'canonical_name': 'fixture.NamesLeft', 'package_identity': 'names-left-v2'}
OWNER = '__objc3_m_ac7ab79fa89fa353a145ed2bbfc7fb456d2641d5174523db859607c3e689b825_'
TAG = OWNER + 'a' * 64


def test_native_template_names_and_local_guards_keep_their_exact_owner():
    names = [f'_Z16private_templateB139{TAG}IiET_S0_',
             f'_ZZ16private_templateB139{TAG}IiET_S0_E10local_bias',
             f'_ZGVZ16private_templateB139{TAG}IiET_S0_E10local_bias',
             f'_Z16private_variableB139{TAG}ILi0EE',
             f'_ZGV16private_variableB139{TAG}ILi0EE']
    qualification.validate_private_cpp_symbols(
        '\n'.join('  Name: ' + name + ' (42)' for name in names), MODULE)


@pytest.mark.parametrize('name', ['_Z16private_templateIiET_S0_',
                                  '_Z16private_variableILi0EE',
                                  '_ZGV16private_variableILi0EE'])
def test_unowned_template_symbols_cannot_hide_behind_another_owned_symbol(name):
    with pytest.raises(ValueError, match='lacks its module/package ABI owner'):
        qualification.validate_private_cpp_symbols('Name: '+TAG+'\nName: '+name, MODULE)


@pytest.mark.parametrize('field', ['canonical_name', 'package_identity'])
def test_template_abi_tag_must_match_both_module_and_package(field):
    foreign = {**MODULE, field: MODULE[field] + '.other'}
    with pytest.raises(ValueError, match='lacks its module/package ABI owner'):
        qualification.validate_private_cpp_symbols('Name: _Z16private_templateB139'+TAG+'IiET_S0_', foreign)


def test_concrete_private_symbols_still_require_renaming():
    with pytest.raises(ValueError, match='namespace declaration retained'):
        qualification.validate_private_cpp_symbols('Name: _Z26private_namespace_helperB139'+TAG+'v', MODULE)
