import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_structured_runtime as qualification


def test_public_abi_matrix_includes_c_and_native_workers():
    modes = qualification.configurations()
    assert len(modes) == len(set(modes)) == 22
    assert {mode[0] for mode in modes} == {'c', 'c++', 'objective-c', 'objective-c++'}
    assert {mode[1] for mode in modes if mode[4]} == {'c++20', 'c++23'}
    assert sum(mode[4] for mode in modes) == 4
    assert {mode[3] for mode in modes} == {'O0', 'O2'}


def test_native_failure_replaces_stale_success_with_bound_diagnostics(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/structured-runtime'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    for name in ('contract.c', 'worker.cpp'):
        (fixture / name).write_text('int main(void) { return 0; }')
    output = tmp_path / 'output'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        kwargs['stdout'].write('forced native compiler failure\n')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='structured runtime failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert len(calls) == 1 and report['status'] == 'FAIL'
    assert report['sdk_seal'] == 'a' * 64
    assert report['commands'][0]['exit_code'] == 1
    assert len(report['commands'][0]['log_sha256']) == 64
