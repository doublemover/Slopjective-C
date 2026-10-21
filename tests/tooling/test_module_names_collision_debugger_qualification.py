import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_module_names_collision_debugger as qualification


def test_collision_debugger_failure_replaces_prior_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    (sdk / 'bin').mkdir(parents=True)
    (sdk / 'bin/objc3c').touch()
    fixture = sdk / 'share/objc3c/examples/module-names'
    for name in ('left/module.m', 'right/module.m', 'app/main.m'):
        path = fixture / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('int debugger_marker(void) { return shared_global; }\n')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'target': 'x86_64-linux-gnu', 'seal_sha256': 'a' * 64}))
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    monkeypatch.setattr(qualification, 'environment', lambda _: {})

    def fail(argv, **kwargs):
        kwargs['stdout'].write('native provider failed\n')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', fail)
    with pytest.raises(ValueError, match='objective-c-c17-arc-left'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['commands'][0]['exit_code'] == 1
    assert report['commands'][0]['log_sha256'] == qualification.packages.digest(output / 'objective-c-c17-arc-left.log')
