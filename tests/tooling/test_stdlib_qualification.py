"""The installed library gate must preserve evidence and reject stale success."""
import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_stdlib as qualification


@pytest.mark.parametrize('failed_command', (1, 2, 3, 4))
def test_failed_library_gate_replaces_success_and_reclaims_temporary_files(
        tmp_path, monkeypatch, failed_command):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/stdlib'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    for name in ('provider.h', 'provider.m', 'client.m', 'factory-contract.m', 'system-contract.m'):
        (fixture / name).write_text('fixture')
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    (sdk / 'bin').mkdir()
    (sdk / 'bin/clang').write_text('compiler')
    (sdk / 'lib').mkdir()
    (sdk / 'lib/libobjc3_runtime_services.a').write_text('runtime')
    calls, roots = [], []

    def run(argv, **kwargs):
        calls.append(argv)
        roots.append(kwargs['cwd'])
        if len(calls) > 1:
            assert not (kwargs['cwd'] / 'provider.m').exists()
        kwargs['stdout'].write('forced failure\n')
        return SimpleNamespace(returncode=int(len(calls) == failed_command))

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='standard library failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert len(report['commands']) == failed_command
    assert report['commands'][-1]['exit_code'] == 1
    assert roots and all(not root.exists() for root in roots)
    record = report['commands'][-1]
    assert record['log_sha256'] == qualification.packages.digest(
        output / (record['name'] + '.log'))
