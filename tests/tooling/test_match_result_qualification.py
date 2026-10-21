"""A failed match/PCH/interface run must never retain reusable PASS evidence."""
import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_match_results as qualification


@pytest.mark.parametrize('failed_command', (1, 3, 5, 94))
def test_compiler_execution_pch_and_source_free_failure_replace_success(
        tmp_path, monkeypatch, failed_command):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/match-results'
    fixture.mkdir(parents=True)
    for name in ('runtime.m', 'provider.m', 'consumer.m'):
        (fixture / name).write_text('fixture')
    (sdk / 'bin').mkdir()
    for name in ('clang', 'objc3c'):
        (sdk / 'bin' / name).write_text('compiler')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls, roots = [], []

    def run(argv, **kwargs):
        calls.append(argv)
        root = kwargs['cwd']
        roots.append(root)
        (root / 'application').write_bytes(b'compiled application')
        if len(calls) == 93 and failed_command == 94:
            (root / 'MatchResults.objc3i.json').write_text(json.dumps({
                'module': {'semantic_fingerprint': 'b' * 64}}))
        if len(calls) == 94:
            assert not (root / 'provider.m').exists()
        if Path(argv[0]).name == 'lldb':
            kwargs['stdout'].write('(int) $0 = 10\n(int) $1 = 1\nProcess 1 exited with status = 0\n')
        else:
            kwargs['stdout'].write('forced qualification failure\n')
        return SimpleNamespace(returncode=int(len(calls) == failed_command))

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='match result qualification failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL'
    assert len(report['commands']) == failed_command
    assert not report['source_free_interface_cases']
    assert roots and all(not root.exists() for root in roots)
    command = report['commands'][-1]
    assert command['exit_code'] == 1
    assert command['log_sha256'] == qualification.packages.digest(
        output / (command['name'] + '.log'))
