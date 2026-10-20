import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_module_names_debugger as qualification


def test_debugger_failure_replaces_stale_success_and_retains_command_evidence(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    (sdk / 'bin').mkdir(parents=True)
    (sdk / 'bin/objc3c').touch()
    fixture = sdk / 'share/objc3c/examples/module-names/debugger'
    fixture.mkdir(parents=True)
    for name in ('provider.m', 'main.m'):
        (fixture / name).write_text('/* fixture */\n')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'target': 'x86_64-linux-gnu', 'seal_sha256': 'a' * 64}))
    output = tmp_path / 'output'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    monkeypatch.setattr(qualification, 'environment', lambda _: {})

    def fail(argv, **kwargs):
        kwargs['stdout'].write('native compile rejected the binding\n')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', fail)
    with pytest.raises(ValueError, match='objective-c-c17-arc-provider'):
        qualification.qualify(sdk, output)
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['commands'][0]['exit_code'] == 1
    log = output / 'objective-c-c17-arc-provider.log'
    assert report['commands'][0]['log_sha256'] == qualification.packages.digest(log)
    assert 'rejected the binding' in log.read_text()
