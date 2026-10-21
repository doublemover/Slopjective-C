import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_native_async as qualification
from scripts.objc3c_toolchain import qualify_native_async_sanitizers as sanitizers
from scripts.objc3c_toolchain import qualify_native_async_packages as native_packages


def test_native_async_matrix_covers_objcpp_standards_and_ownership():
    modes = qualification.configurations()
    assert len(modes) == len(set(modes)) == 8
    assert {mode[1] for mode in modes} == {'c++20', 'c++23'}
    assert {mode[2] for mode in modes if mode[0] == 'objective-c++'} == {'arc', 'mrc'}
    assert {mode[3] for mode in modes} == {'O0', 'O2'}
    executions = qualification.execution_configurations()
    assert len(executions) == len(set(executions)) == 16
    assert {(mode[2], mode[3]) for mode in executions} == {
        ('arc', 'arc'), ('mrc', 'mrc'), ('arc', 'mrc'), ('mrc', 'arc')}


def test_failed_native_async_provider_replaces_stale_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/native-async'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    for name in ('provider.cpp', 'provider.h', 'client.cpp'):
        (fixture / name).write_text('int main() {}')
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        kwargs['stdout'].write('forced native async provider failure\n')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='native async functions failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['sdk_seal'] == 'a' * 64
    assert len(calls) == 1 and '-fcoro-aligned-allocation' in calls[0]
    assert '-fobjc-version=3' in calls[0]
    assert report['commands'][0]['exit_code'] == 1


def test_native_async_sanitizers_cover_owned_and_unowned_objcpp():
    modes = sanitizers.configurations()
    assert len(modes) == len(set(modes)) == 8
    assert {mode[2] for mode in modes} == {'arc', 'mrc'}
    assert {mode[0] for mode in modes} == {'address,undefined', 'thread'}
    executions = sanitizers.execution_configurations()
    assert len(executions) == len(set(executions)) == 16
    assert {(mode[2], mode[3]) for mode in executions} == {
        ('arc', 'arc'), ('mrc', 'mrc'), ('arc', 'mrc'), ('mrc', 'arc')}


def test_native_async_package_failure_replaces_stale_success(tmp_path, monkeypatch):
    sdk, output = tmp_path / 'sdk', tmp_path / 'evidence'
    sdk.mkdir()
    output.mkdir()
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')

    def fail(*args, **kwargs):
        raise ValueError('forced native async source package failure')

    monkeypatch.setattr(native_packages, 'prepare', fail)
    with pytest.raises(ValueError, match='forced native async source package failure'):
        native_packages.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['sdk_seal'] == 'a' * 64


@pytest.mark.parametrize("failed_command", [0, 1])
def test_native_async_sanitizer_failure_preserves_report(tmp_path, monkeypatch, failed_command):
    compiler, resources, output = (tmp_path / name for name in ('compiler', 'resources', 'output'))
    compiler.mkdir()
    resources.mkdir()
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []
    workspaces = []

    def run(argv, **kwargs):
        calls.append(argv)
        kwargs['stdout'].write('forced native async sanitizer failure\n')
        if kwargs.get('cwd'):
            workspaces.append(kwargs['cwd'].parent)
            (kwargs['cwd'] / 'partial.o').write_bytes(b'partial object')
        return SimpleNamespace(returncode=int(len(calls) > failed_command))

    monkeypatch.setattr(sanitizers.subprocess, 'run', run)
    with pytest.raises(ValueError, match='address-undefined-O0-arc-'):
        sanitizers.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['executions']
    assert len(calls) == failed_command + 1
    assert '-fsanitize=address,undefined' in calls[0]
    assert '-fobjc-version=3' not in calls[0]
    if failed_command:
        assert '-fobjc-version=3' in calls[1] and 'objective-c++' in calls[1]
    assert workspaces and all(path != output for path in workspaces)
    assert all(not path.exists() for path in workspaces)
    log = output / report['commands'][-1]['name']
    assert report['commands'][-1]['log_sha256'] == sanitizers.digest(log.with_suffix('.log'))


def test_checked_async_package_requires_combined_typed_effect():
    assert native_packages.has_checked_async({
        'declarations': [{'effects': {'async': True, 'throws': True,
                                      'throws_kind': 'typed'}}]})
    assert not native_packages.has_checked_async([
        {'effects': {'async': True, 'throws': False, 'throws_kind': ''}},
        {'effects': {'async': False, 'throws': True, 'throws_kind': 'typed'}}])
    assert not native_packages.has_checked_async({
        'async': True, 'throws': True, 'throws_kind': 'untyped'})
