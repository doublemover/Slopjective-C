"""Lock detector disposition/location controls; native CTest runs the real compiler."""
import json
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_toolchain import qualify_executor_locks as locks

ROOT = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('fault', ['crash', 'timeout', 'other-exit', 'clean-exit', 'missing-checker',
                                  'wrong-location', 'unrelated-error', 'empty-diagnostic'])
def test_an_unrelated_failure_cannot_satisfy_lock_detection(tmp_path, fault):
    source = tmp_path / 'control.cpp'; source.write_text('// expected-site\n')
    text = str(source)+":1:3: error: writing variable 'stopping' requires holding mutex [-Werror,-Wthread-safety-analysis]\n"
    code = {'crash': -11, 'timeout': 124, 'other-exit': 2, 'clean-exit': 0}.get(fault, 1)
    if fault == 'missing-checker':
        text = text.replace(' [-Werror,-Wthread-safety-analysis]', '')
    elif fault == 'wrong-location':
        text = text.replace(':1:3:', ':2:3:')
    elif fault == 'unrelated-error':
        text += str(source)+':2:1: fatal error: missing generated header\n'
    elif fault == 'empty-diagnostic':
        text = ''
    with pytest.raises(ValueError):
        locks.validate_diagnostic(source, code, text, expected="writing variable 'stopping'",
                                  marker='expected-site')


def test_corrected_control_and_specific_detector_failure_are_distinct(tmp_path):
    source = tmp_path / 'control.cpp'; source.write_text('// expected-site\n')
    locks.validate_diagnostic(source, 0, '')
    text = str(source)+":1:3: error: writing variable 'stopping' requires holding mutex [-Werror,-Wthread-safety-analysis]\n"
    locks.validate_diagnostic(source, 1, text, expected="writing variable 'stopping'", marker='expected-site')
    with pytest.raises(ValueError):
        locks.validate_diagnostic(source, 0, 'warning: unexpected warning')


@pytest.mark.parametrize('failure', ['tool-mismatch', 'probe-timeout'])
def test_tool_admission_and_probe_timeout_replace_stale_success(tmp_path, monkeypatch, failure):
    compiler = tmp_path / 'clang++'; compiler.write_bytes(b'fixture compiler')
    output = tmp_path / 'evidence'; output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    monkeypatch.setattr(locks.subprocess, 'check_output', lambda *args, **kwargs:
                        'clang version ' + ('22.0.0' if failure == 'tool-mismatch' else '23.1.3'))
    def run(*args, **kwargs):
        if failure == 'tool-mismatch':
            pytest.fail('must not compile with the mismatched tool')
        raise subprocess.TimeoutExpired(args[0], 60)
    monkeypatch.setattr(locks.subprocess, 'run', run)
    with pytest.raises((ValueError, subprocess.TimeoutExpired)):
        locks.qualify(ROOT, compiler, output)
    assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'
