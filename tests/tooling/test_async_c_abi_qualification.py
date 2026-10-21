import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_async_c_abi as qualification


@pytest.mark.parametrize('failure_stage', [0, 1])
def test_failed_c_or_cpp_compilation_cannot_leave_reusable_success(
        tmp_path, monkeypatch, failure_stage):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/async-c-abi'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    for name in ('bridge.h', 'bridge.c', 'client.cpp'):
        (fixture / name).write_text('fixture')
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        if len(calls) > 1:
            assert not (kwargs['cwd'] / 'bridge.c').exists()
        kwargs['stdout'].write('forced independent language compilation failure\n')
        return SimpleNamespace(returncode=int(len(calls) > failure_stage))

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='typed async C ABI failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['sdk_seal'] == 'a' * 64
    assert len(calls) == failure_stage + 1
    assert '-std=c17' in calls[0] and '-std=c++20' not in calls[0]
    if failure_stage:
        assert '-std=c++20' in calls[1] and '-std=c17' not in calls[1]
    command = report['commands'][-1]
    assert command['exit_code'] == 1
    assert command['log_sha256'] == qualification.packages.digest(
        output / (command['name'] + '.log'))
