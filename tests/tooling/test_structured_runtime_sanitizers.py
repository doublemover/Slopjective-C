import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import qualify_structured_sanitizers as qualification


@pytest.fixture(autouse=True)
def product_boundary(monkeypatch):
    # These tests exercise owner-level failure propagation; the shared process
    # owner has separate real compiler/loader/runtime mutation controls.
    class Products:
        def __init__(self, *args, **kwargs): pass
        def record(self, gate, name, argv, rows, *, cwd, log):
            with log.open('w') as stream:
                result = qualification.subprocess.run(list(map(str, argv)), cwd=cwd,
                    env=gate.environment, stdout=stream, stderr=qualification.subprocess.STDOUT)
            rows.append({'name': name, 'exit_code': result.returncode, 'log': str(log),
                         'log_sha256': qualification.digest(log)})
            if result.returncode:
                raise ValueError('instrumented structured runtime failed: '+name)
        def finalize(self): pytest.fail('a failed owner reached product promotion')
    monkeypatch.setattr(qualification, 'ProductCommands', Products)


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

    class Profiles:
        def __init__(self, *args, **kwargs): pass
        def get(self, *args, **kwargs):
            return SimpleNamespace(archive=tmp_path / 'canonical-services.a', objc_runtime=tmp_path / 'libobjc.so')
    monkeypatch.setattr(qualification, 'RuntimeProfiles', Profiles)
    controls = []
    class Gate:
        environment = {}
        def __init__(self, *args, **kwargs): pass
        def ensure_controls(self, optimization): controls.append(optimization)
    monkeypatch.setattr(qualification, 'SanitizerQualification', Gate)
    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError, match='address-undefined-O0-compile-0'):
        qualification.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and not report['executions']
    assert controls == ['O0']
    assert len(calls) == 1 and '-fsanitize=address,undefined' in calls[0]
    assert str(tmp_path / 'canonical-services.a') not in calls[0]
    assert not any(str(path).endswith('/concurrency/service_public.cpp') for path in calls[0])
    assert '-DOBJC3_ACTOR_SCHEDULING_REGRESSIONS' in calls[0]
    assert any(str(path).endswith('/structured_runtime/actor_scheduling.cpp') for path in calls[0])
    executable = Path(calls[0][-1])
    assert executable.parent != output
    assert not executable.parent.exists()
    assert report['commands'][0]['exit_code'] == 1
    assert report['commands'][0]['log_sha256'] == qualification.digest(output / 'address-undefined-O0-compile-0.log')


def test_detector_failure_prevents_project_compilation_and_replaces_pass(tmp_path, monkeypatch):
    compiler, resources, output = (tmp_path / name for name in ('compiler', 'resources', 'output'))
    for path in (compiler, resources, output): path.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    class Profiles:
        def __init__(self, *args, **kwargs): pass
        def get(self, *args, **kwargs):
            return SimpleNamespace(archive=tmp_path / 'canonical-services.a', objc_runtime=tmp_path / 'libobjc.so')
    monkeypatch.setattr(qualification, 'RuntimeProfiles', Profiles)
    class Gate:
        environment = {}
        def __init__(self, *args, **kwargs): pass
        def ensure_controls(self, optimization): raise ValueError('detector disabled')
    monkeypatch.setattr(qualification, 'SanitizerQualification', Gate)
    def forbidden(*args, **kwargs): pytest.fail('project compilation preceded detector qualification')
    monkeypatch.setattr(qualification.subprocess, 'run', forbidden)
    with pytest.raises(ValueError, match='detector disabled'):
        qualification.qualify(tmp_path, compiler, resources, output)
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and report['commands'] == []


def test_wrong_loaded_native_dependency_prevents_runtime_execution(tmp_path, monkeypatch):
    compiler, resources, output = (tmp_path / name for name in ('compiler', 'resources', 'output'))
    compiler.mkdir(); resources.mkdir()
    dependency = tmp_path / 'libobjc.so'; dependency.write_bytes(b'qualified dependency fixture')
    archive = tmp_path / 'services.a'; archive.write_bytes(b'qualified archive fixture')
    class Profiles:
        def __init__(self, *args, **kwargs): pass
        def get(self, *args, **kwargs):
            return SimpleNamespace(archive=archive, objc_runtime=dependency)
    class Gate:
        environment = {}
        loader = tmp_path / 'loader'
        def __init__(self, *args, **kwargs): pass
        def ensure_controls(self, optimization): pass
    monkeypatch.setattr(qualification, 'RuntimeProfiles', Profiles)
    monkeypatch.setattr(qualification, 'SanitizerQualification', Gate)
    commands = []
    def run(argv, **kwargs):
        commands.append(argv)
        if '-c' not in argv:
            assert '-Wl,--no-as-needed' in argv
        Path(argv[-1]).write_bytes(b'compiled program fixture')
        return SimpleNamespace(returncode=0)
    def loaded(binary, loader, libraries, *, required, run):
        assert loader == Gate.loader and libraries == [dependency]
        assert required == (dependency.resolve(),)
        raise ValueError('wrong loaded native runtime')
    monkeypatch.setattr(qualification.subprocess, 'run', run)
    monkeypatch.setattr(qualification, 'loaded_graph', loaded)
    with pytest.raises(ValueError, match='wrong loaded native runtime'):
        qualification.qualify(tmp_path, compiler, resources, output)
    receipt = json.loads((output / 'qualification.json').read_text())
    assert receipt['status'] == 'FAIL' and receipt['executions'] == []
    assert len(commands) == 3 and all(not Path(command[-1]).exists() for command in commands)
