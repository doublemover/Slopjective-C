import json
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_carrier_debugger as qualification


def test_transcript_rejects_payload_child_on_none():
    plan = [('frame variable absent',
             {'summaries': {'absent': 'None'}, 'absent': ['value =']})]
    qualification._check_transcript(
        '(lldb) frame variable absent\n(Number) absent = None\n', plan)
    with pytest.raises(ValueError, match='inactive/out-of-scope'):
        qualification._check_transcript(
            '(lldb) frame variable absent\n'
            '(Number) absent = None {\n  value = -1515870811\n}\n', plan)


def test_void_result_names_are_not_mistaken_for_payload_children():
    plan = [('frame variable unit_error both_error', {
        'summaries': {'unit_error': 'Err(void)', 'both_error': 'Err(void)'},
        'absent': ['error =']})]
    qualification._check_transcript(
        '(lldb) frame variable unit_error both_error\n'
        '(EmptyError) unit_error = Err(void)\n'
        '(UnitResult) both_error = Err(void)\n', plan)
    with pytest.raises(ValueError, match='did not execute'):
        qualification._check_transcript('(lldb) run\nProcess stopped\n', plan)


def test_failed_compile_replaces_stale_success_and_keeps_command_log(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    (sdk / 'bin').mkdir(parents=True)
    (sdk / 'lib').mkdir()
    for name in ('objc3c', 'clang', 'clang++', 'lldb', 'llvm-dwarfdump'):
        (sdk / 'bin' / name).touch()
    (sdk / 'lib/libobjc.so').touch()
    fixture = sdk / 'share/objc3c/examples/carrier-debugger'
    fixture.mkdir(parents=True)
    for name in ('main.m', 'values.h'):
        (fixture / name).write_text('/* native debugger fixture */\n')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    output = tmp_path / 'output'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    monkeypatch.setattr(qualification, 'environment', lambda _: {})

    def fail(argv, **kwargs):
        kwargs['stdout'].write('native source binding compilation failed\n')
        return SimpleNamespace(wait=lambda timeout: 1)

    monkeypatch.setattr(qualification.subprocess, 'Popen', fail)
    with pytest.raises(ValueError, match='c17-mrc-compile'):
        qualification.qualify(sdk, output)
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert not report['profile_conformance_claimed']
    assert report['commands'][0]['exit_code'] == 1
    log = output / 'c17-mrc-compile.log'
    assert report['commands'][0]['log_sha256'] == qualification.packages.digest(log)
    assert 'source binding compilation failed' in log.read_text()
