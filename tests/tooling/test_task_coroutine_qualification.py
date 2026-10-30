import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_task_coroutines as qualification
from scripts.objc3c_toolchain import qualify_task_coroutine_sanitizers as sanitizers


def _write_bound_inputs(sdk):
    for relative in ('bin/clang', 'bin/objc3c', 'lib/libobjc3_runtime_services.a',
                     'lib/libobjc.so', 'include/objc3c/runtime/objc3_task.hpp',
                     'include/objc3c/runtime/objc3_runtime_services.h'):
        path = sdk / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('bound input')


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
    _write_bound_inputs(sdk)
    for name in ('provider.cpp', 'provider.h', 'client.cpp', 'group_await.cpp'):
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


def _mock_canonical_profiles(monkeypatch, output):
    archive = output / 'canonical-services.a'
    dependency = output / 'libobjc.so'
    archive.write_bytes(b'qualified test archive')
    dependency.write_bytes(b'qualified test dependency')
    selected, controls = [], []

    class Profiles:
        def __init__(self, *args, **kwargs):
            pass

        def get(self, mode, optimization, *, objc_runtime=None):
            selected.append((mode, optimization, objc_runtime))
            return SimpleNamespace(archive=archive, objc_runtime=dependency)

    class Policy:
        environment = {'OBJC3_TEST_STRICT_POLICY': 'owned'}

        def __init__(self, *args, libraries, **kwargs):
            assert libraries == (archive, dependency)

        def ensure_controls(self, optimization):
            controls.append(optimization)

    monkeypatch.setattr(sanitizers, 'RuntimeProfiles', Profiles)
    monkeypatch.setattr(sanitizers, 'SanitizerQualification', Policy)
    return selected, controls, archive, dependency


@pytest.mark.parametrize('failed_command', [0, 1])
def test_instrumented_failure_preserves_diagnostics(tmp_path, monkeypatch, failed_command):
    compiler, resources, output = (tmp_path / name for name in ('compiler', 'resources', 'output'))
    compiler.mkdir()
    resources.mkdir()
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    selected, controls, archive, dependency = _mock_canonical_profiles(monkeypatch, output)
    calls, workspaces = [], []

    def run(argv, **kwargs):
        assert controls == ['O0']
        assert kwargs['env'] == {'OBJC3_TEST_STRICT_POLICY': 'owned'}
        calls.append(argv)
        kwargs['stdout'].write('forced sanitizer failure\n')
        if '-o' in argv:
            executable = Path(argv[argv.index('-o') + 1])
            workspaces.append(executable.parent)
            executable.write_bytes(b'partial executable')
        return SimpleNamespace(returncode=int(len(calls) > failed_command))

    monkeypatch.setattr(sanitizers.subprocess, 'run', run)
    monkeypatch.setattr(sanitizers, 'configurations', lambda: [('address,undefined', 'O0', 'arc')])
    with pytest.raises(ValueError, match='address-undefined-O0-arc-'):
        sanitizers.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['executions']
    assert selected == [('address,undefined', 'O0', None)]
    assert len(calls) == failed_command + 1
    assert '-fsanitize=address,undefined' in calls[0]
    assert '-fno-sanitize-recover=undefined' in calls[0]
    assert '-fcoro-aligned-allocation' in calls[0]
    assert '-fobjc-version=3' in calls[0] and 'objective-c++' in calls[0]
    assert str(archive) in calls[0] and str(dependency) in calls[0]
    if failed_command:
        assert calls[1] == [str(workspaces[0] / 'address-undefined-O0-arc')]
    assert workspaces and all(not path.exists() for path in workspaces)
    assert archive.is_file() and dependency.is_file()
    log = output / (report['commands'][-1]['name'] + '.log')
    assert report['commands'][-1]['exit_code'] == 1
    assert report['commands'][-1]['log_sha256'] == sanitizers.digest(log)


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
    _write_bound_inputs(sdk)
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


@pytest.mark.parametrize('changed_input', ['runtime', 'compiler', 'fixture'])
def test_coroutine_input_mutation_replaces_stale_pass(tmp_path, monkeypatch, changed_input):
    sdk, fixture, output = (tmp_path / name for name in ('sdk', 'fixtures', 'evidence'))
    fixture.mkdir()
    output.mkdir()
    for name in ('provider.cpp', 'provider.h', 'client.cpp', 'group_await.cpp'):
        (fixture / name).write_text('original fixture')
    for relative in ('bin/clang', 'lib/libobjc3_runtime_services.a', 'lib/libobjc.so',
                     'include/objc3c/runtime/objc3_task.hpp',
                     'include/objc3c/runtime/objc3_runtime_services.h'):
        path = sdk / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('original input')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({'seal_sha256': 'a' * 64}))
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    target = {'runtime': sdk / 'lib/libobjc3_runtime_services.a',
              'compiler': sdk / 'bin/clang',
              'fixture': fixture / 'group_await.cpp'}[changed_input]
    commands = []

    def run(argv, **kwargs):
        commands.append(argv)
        kwargs['stdout'].write('successful simulated command\n')
        if '-o' in argv:
            Path(argv[argv.index('-o') + 1]).write_text('compiled artifact')
        if len(argv) == 1 and str(argv[0]).endswith('/group-await'):
            target.write_text('changed input')
        return SimpleNamespace(returncode=0)

    monkeypatch.setattr(qualification, 'configurations',
                        lambda: [('c++', 'c++20', 'none', 'O2')])
    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='inputs changed during qualification'):
        qualification.qualify(sdk, output, fixture=fixture, compiler=sdk / 'bin/clang')
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL'
    assert len(commands) == len(report['commands']) == 9
    assert len(report['cases']) == 1
    assert all(command['exit_code'] == 0 for command in report['commands'])
