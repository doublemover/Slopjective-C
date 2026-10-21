import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_structured_sanitizers as qualification


def test_instrumented_runtime_failure_replaces_prior_success(tmp_path, monkeypatch):
    compiler = tmp_path / 'compiler'
    resources = tmp_path / 'resources'
    compiler.mkdir()
    resources.mkdir()
    output = tmp_path / 'evidence'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        kwargs['stdout'].write('sanitizer compilation failed\n')
        Path(argv[-1]).write_bytes(b'partial executable')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='address-undefined-O0-build'):
        qualification.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['executions']
    assert len(calls) == 1 and '-fsanitize=address,undefined' in calls[0]
    assert any(str(path).endswith('/concurrency/service_public.cpp') for path in calls[0])
    executable = Path(calls[0][-1])
    assert executable.parent != output
    assert not executable.parent.exists()
    assert report['commands'][0]['exit_code'] == 1
    assert report['commands'][0]['log_sha256'] == qualification.digest(output / 'address-undefined-O0-build.log')
