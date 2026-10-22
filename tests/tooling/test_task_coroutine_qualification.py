import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_task_coroutines as qualification
from scripts.objc3c_toolchain import qualify_task_coroutine_sanitizers as sanitizers


def test_native_coroutine_matrix_covers_ownership_and_optimization():
    modes = qualification.configurations()
    assert len(modes) == len(set(modes)) == 12
    assert {mode[1] for mode in modes} == {'c++20', 'c++23'}
    assert {mode[2] for mode in modes if mode[0] == 'objective-c++'} == {'arc', 'mrc'}
    assert {mode[3] for mode in modes} == {'O0', 'O2'}


def test_failed_native_provider_replaces_stale_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/task-coroutines'
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
        kwargs['stdout'].write('forced provider failure\n')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='task coroutines failed'):
        qualification.qualify(sdk, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['sdk_seal'] == 'a' * 64
    assert len(calls) == 1 and '-fcoro-aligned-allocation' in calls[0]
    assert report['commands'][0]['exit_code'] == 1


def test_instrumented_failure_preserves_diagnostics(tmp_path, monkeypatch):
    compiler, resources, output = (tmp_path / name for name in ('compiler', 'resources', 'output'))
    compiler.mkdir()
    resources.mkdir()
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        kwargs['stdout'].write('forced sanitizer failure\n')
        Path(argv[-1]).write_bytes(b'partial executable')
        return SimpleNamespace(returncode=1)

    monkeypatch.setattr(sanitizers.subprocess, 'run', run)
    with pytest.raises(ValueError, match='address-undefined-O0-none-build'):
        sanitizers.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['executions']
    assert len(calls) == 1 and '-fsanitize=address,undefined' in calls[0]
    assert '-fcoro-aligned-allocation' in calls[0]
    executable = Path(calls[0][-1])
    assert executable.parent != output
    assert not executable.parent.exists()
    assert report['commands'][0]['log_sha256'] == sanitizers.digest(output / 'address-undefined-O0-none-build.log')


def test_sanitizers_instrument_arc_mrc_and_native_cpp():
    modes = sanitizers.configurations()
    assert len(modes) == len(set(modes)) == 12
    assert {mode[2] for mode in modes} == {'none', 'arc', 'mrc'}
    assert {mode[0] for mode in modes} == {'address,undefined', 'thread'}


def test_failed_group_await_witness_cannot_publish_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    fixture = sdk / 'share/objc3c/examples/task-coroutines'
    fixture.mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'b' * 64}))
    for name in ('provider.cpp', 'provider.h', 'client.cpp', 'group_await.cpp'):
        (fixture / name).write_text('int main() {}')
    output = tmp_path / 'evidence'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    calls = []

    def run(argv, **kwargs):
        calls.append(argv)
        if '-o' in argv:
            Path(argv[argv.index('-o') + 1]).write_bytes(b'executable')
        failure = any(str(argument).endswith('group_await.cpp') for argument in argv)
        kwargs['stdout'].write('forced group-await failure\n' if failure else 'ok\n')
        return SimpleNamespace(returncode=int(failure))

    monkeypatch.setattr(qualification, 'configurations',
                        lambda: [('c++', 'c++20', 'none', 'O2')])
    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='group-await-compile'):
        qualification.qualify(sdk, output)
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL' and not report['cases']
    assert report['commands'][-1]['name'].endswith('group-await-compile')
    assert report['commands'][-1]['exit_code'] == 1
    # The original source-free provider path must still run before this witness.
    assert len(calls) == 6
    assert not Path(calls[-1][calls[-1].index('-o') + 1]).parent.exists()
