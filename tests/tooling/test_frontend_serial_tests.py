import json
from pathlib import Path

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
                        lambda *a, **kw: json.dumps({'tests': [{'name': r['name']} for r in rows]}))
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
