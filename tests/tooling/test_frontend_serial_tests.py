import json
from pathlib import Path
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain.frontend_tests import qualify


def fixture(tmp_path, monkeypatch):
    build = tmp_path / "build with ' spaces"
    build.mkdir()
    rows = [{'name': 'objc3_first', 'target': 'objc3_first_test',
             'executable': str(build / 'objc3_first_test')},
            {'name': 'objc3_second', 'target': 'objc3_second_test',
             'executable': str(build / 'objc3_second_test')}]
    (build / 'frontend-test-targets.json').write_text(json.dumps({'tests': rows}))
    monkeypatch.setattr('scripts.objc3c_toolchain.frontend_tests.subprocess.check_output',
                        lambda *a, **kw: json.dumps({'tests': [
                            {'name': r['name'], **({'command': [r['executable']]}
                             if Path(r['executable']).exists() else {})} for r in rows]}))
    return build, rows


def test_links_and_runs_one_test_at_a_time(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    calls = []
    def run(label, args):
        calls.append((label, args))
        if args[0] == 'cmake':
            assert not list(build.glob('*_test'))
            (build / args[4]).write_bytes(b'passed program')
        else:
            assert len(list(build.glob('*_test'))) == 1
    result = qualify(build, tmp_path / 'proof', run, jobs=4)
    assert result['status'] == 'PASS' and result['frontend_ctests'] == 2
    assert result['test_executables'] == 2 and len(calls) == 4
    assert calls[1][1][-1] == '^objc3_first$'
    assert not list(build.glob('*_test'))


def test_failed_test_preserves_its_executable_and_receipt(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    def run(label, args):
        if args[0] == 'cmake':
            (build / args[4]).write_bytes(b'failed program')
        else:
            raise RuntimeError('CTest failed')
    with pytest.raises(RuntimeError, match='CTest failed'):
        qualify(build, tmp_path / 'proof', run, jobs=2)
    assert Path(rows[0]['executable']).read_bytes() == b'failed program'
    receipt = json.loads((tmp_path / 'proof/qualification.json').read_text())
    assert receipt['status'] == 'FAIL' and not receipt['reclaimed']


def test_manifest_drift_rejected_before_building(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    (build / 'frontend-test-targets.json').write_text(json.dumps({'tests': rows[:1]}))
    with pytest.raises(ValueError, match='CTest inventory'):
        qualify(build, tmp_path / 'proof', lambda *a: pytest.fail('must not build'), jobs=2)


def test_outside_or_symlink_target_preserved(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    outside = tmp_path / rows[0]['target']
    outside.write_bytes(b'keep')
    Path(rows[0]['executable']).symlink_to(outside)
    with pytest.raises(ValueError, match='owned build output'):
        qualify(build, tmp_path / 'proof', lambda *a: pytest.fail('must not build'), jobs=2)
    assert outside.read_bytes() == b'keep'


def test_corrupt_inventory_replaces_stale_success(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    output = tmp_path / 'proof'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    (build / 'frontend-test-targets.json').write_text('{}')
    with pytest.raises(ValueError, match='CTest inventory'):
        qualify(build, output, lambda *a: pytest.fail('must not build'), jobs=2)
    assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'


def test_compiler_and_object_state_are_preserved(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    for name in ('objc3c', 'clang-scan-deps', 'frontend.cpp.o', 'libfrontend.a'):
        (build / name).write_bytes(b'keep')
    def run(label, args):
        if args[0] == 'cmake':
            (build / args[4]).write_bytes(b'passed program')
    qualify(build, tmp_path / 'proof', run, jobs=4)
    for name in ('objc3c', 'clang-scan-deps', 'frontend.cpp.o', 'libfrontend.a'):
        assert (build / name).read_bytes() == b'keep'


def test_shared_target_runs_all_profiles_before_reclaiming(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    rows[1].update(target=rows[0]['target'], executable=rows[0]['executable'])
    (build / 'frontend-test-targets.json').write_text(json.dumps({'tests': rows}))
    calls = []
    def run(label, args):
        calls.append(args)
        if args[0] == 'cmake':
            Path(rows[0]['executable']).write_bytes(b'shared program')
        else:
            assert Path(rows[0]['executable']).is_file()
    result = qualify(build, tmp_path / 'proof', run, jobs=2)
    assert result['frontend_ctests'] == 2 and result['test_executables'] == 1
    assert [call[0] for call in calls] == ['cmake', 'ctest', 'ctest']
    assert {row['name'] for row in result['tests']} == {'objc3_first', 'objc3_second'}
    assert not Path(rows[0]['executable']).exists()


def test_failed_variant_preserves_the_shared_executable(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    rows[1].update(target=rows[0]['target'], executable=rows[0]['executable'])
    (build / 'frontend-test-targets.json').write_text(json.dumps({'tests': rows}))
    def run(label, args):
        if args[0] == 'cmake': Path(rows[0]['executable']).write_bytes(b'shared program')
        elif label.endswith('objc3_second-ctest'): raise RuntimeError('second profile failed')
    with pytest.raises(RuntimeError, match='second profile'):
        qualify(build, tmp_path / 'proof', run, jobs=2)
    receipt = json.loads((tmp_path / 'proof/qualification.json').read_text())
    assert receipt['status'] == 'FAIL' and len(receipt['tests']) == 1
    assert not receipt['reclaimed'] and Path(rows[0]['executable']).is_file()


def test_rebound_ctest_command_is_rejected_before_execution(tmp_path, monkeypatch):
    build, rows = fixture(tmp_path, monkeypatch)
    monkeypatch.setattr('scripts.objc3c_toolchain.frontend_tests.subprocess.check_output',
        lambda *a, **kw: json.dumps({'tests': [
            {'name': row['name'], 'command': ['/foreign/program']} for row in rows]}))
    def run(label, args):
        assert args[0] == 'cmake'
        Path(rows[0]['executable']).write_bytes(b'owned program')
    with pytest.raises(ValueError, match='command differs'):
        qualify(build, tmp_path / 'proof', run, jobs=2)
    assert Path(rows[0]['executable']).is_file()


def configure_real_project(tmp_path, *, unregistered=False):
    root = Path(__file__).resolve().parents[2]
    source = tmp_path / 'source';source.mkdir()
    (source / 'main.cpp').write_text('int main() { return 0; }\n')
    (source / 'wrapper.py').write_text('import subprocess, sys\nsubprocess.run([sys.argv[1]], check=True)\n')
    registration = 'add_test' if unregistered else 'objc3c_add_frontend_test'
    (source / 'CMakeLists.txt').write_text(f'''
cmake_minimum_required(VERSION 3.25)
project(FrontendInventoryControl LANGUAGES CXX)
enable_testing()
set(_objc3_repo "{root}")
add_library(objc3c_build_defaults INTERFACE)
include("{root}/native/objc3c/cmake/FrontendTests.cmake")
add_executable(objc3_shared_test main.cpp)
{registration}(NAME objc3_shared COMMAND objc3_shared_test)
objc3c_add_frontend_test(NAME objc3_shared_assertions COMMAND objc3_shared_test --assertions)
objc3c_add_frontend_test(NAME objc3_shared_no_rtti COMMAND objc3_shared_test -fno-rtti)
add_executable(objc3_text_file_cli main.cpp)
objc3c_add_frontend_test(NAME objc3_text_file_cli TARGET objc3_text_file_cli
  COMMAND "{sys.executable}" "{source}/wrapper.py" "$<TARGET_FILE:objc3_text_file_cli>")
objc3c_write_frontend_test_inventory()
''')
    build = tmp_path / "build with ' spaces"
    result = subprocess.run(['cmake', '-S', str(source), '-B', str(build), '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Release'], capture_output=True, text=True, timeout=60)
    return build, result


def test_real_cmake_records_shared_profile_commands_before_binaries_exist(tmp_path):
    build, configured = configure_real_project(tmp_path)
    assert configured.returncode == 0, configured.stdout + configured.stderr
    plan = json.loads((build / 'frontend-test-targets.json').read_text())['tests']
    assert len(plan) == 4 and {row['target'] for row in plan} == {'objc3_shared_test', 'objc3_text_file_cli'}
    calls = []
    def run(label, args):
        calls.append(args)
        subprocess.run(list(map(str, args)), check=True, capture_output=True, timeout=60)
    result = qualify(build, tmp_path / 'proof', run, jobs=2)
    assert result['frontend_ctests'] == 4 and result['test_executables'] == 1
    assert sum(call[0] == 'cmake' for call in calls) == 2
    assert sum(call[0] == 'ctest' for call in calls) == 4
    assert (build / 'objc3_text_file_cli').is_file()


def test_real_cmake_refuses_unregistered_target_ownership(tmp_path):
    _, result = configure_real_project(tmp_path, unregistered=True)
    assert result.returncode != 0
    assert 'has no registered executable owner' in result.stdout + result.stderr


def test_source_coverage_uses_the_registered_profile_owner(tmp_path):
    from scripts.objc3c_toolchain.source_coverage import selected_targets
    (tmp_path / 'frontend-test-targets.json').write_text(json.dumps({'tests': [
        {'name': 'objc3_standard_library_assertions', 'target': 'objc3_standard_library_provider_test'}]}))
    assert selected_targets(tmp_path, 'frontend', ['objc3_standard_library_assertions']) == {
        'objc3_standard_library_assertions': 'objc3_standard_library_provider_test'}
    with pytest.raises(ValueError, match='registered frontend target owner'):
        selected_targets(tmp_path, 'frontend', ['objc3_missing'])
