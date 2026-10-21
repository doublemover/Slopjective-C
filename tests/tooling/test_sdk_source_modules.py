"""Content reuse must survive paths and timestamps without trusting stale outputs."""
import json
import os
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.sdk_example import dependencies, digest, environment, hashes, reusable, write_json


def test_dependency_file_unescapes_linux_filenames(tmp_path):
    names = ['source with spaces.m', 'header#name.h', 'cash$name.h', 'back\\slash.h',
             "apostrophe's.h", 'double"quote.h', 'back\\ space.h', 'back\\#hash.h']
    encoded = ['source\\ with\\ spaces.m', 'header\\#name.h', 'cash$$name.h', 'back\\slash.h',
               "apostrophe's.h", 'double"quote.h', 'back\\\\\\ space.h', 'back\\\\#hash.h']
    depfile = tmp_path / 'input.d'
    depfile.write_text('dependencies: \\\n  ' + ' \\\n  '.join(encoded) + '\n')
    assert dependencies(depfile, tmp_path) == [(tmp_path / name).resolve() for name in names]


@pytest.mark.parametrize('group', ['outputs', 'dependencies'])
@pytest.mark.parametrize('damage', ['preserved-mtime', 'missing'])
def test_reuse_checks_every_input_and_output_by_content(tmp_path, group, damage):
    source = tmp_path / 'source.m'
    obj = tmp_path / 'source.o'
    source.write_text('source')
    obj.write_text('object')
    record = {'key': 'qualified-key', 'outputs': hashes([obj]), 'dependencies': hashes([source])}
    assert reusable(record, 'qualified-key')
    assert not reusable(record, 'changed-sdk-or-flags')
    path = Path(next(iter(record[group])))
    if damage == 'missing':
        path.unlink()
    else:
        stamp = path.stat()
        path.write_bytes(b'X' * stamp.st_size)
        os.utime(path, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert not reusable(record, 'qualified-key')


@pytest.mark.parametrize('record', [{}, {'key': 'key'}, {'key': 'key', 'outputs': {}, 'dependencies': {}}])
def test_incomplete_stage_is_never_reused(record):
    assert not reusable(record, 'key')


def test_atomic_record_replaces_complete_json(tmp_path):
    path = tmp_path / 'build-report.json'
    write_json(path, {'before': 'first build'})
    write_json(path, {'after': 'qualified build'})
    assert json.loads(path.read_text()) == {'after': 'qualified build'}
    assert list(tmp_path.iterdir()) == [path]


def test_module_environment_uses_only_explicit_sdk_tools_and_headers(tmp_path, monkeypatch):
    for name in ('CPATH', 'LIBRARY_PATH', 'LD_PRELOAD', 'CCC_OVERRIDE_OPTIONS', 'SDKROOT'):
        monkeypatch.setenv(name, 'ambient-override')
    env = environment(tmp_path)
    assert not {'CPATH', 'LIBRARY_PATH', 'LD_PRELOAD', 'CCC_OVERRIDE_OPTIONS', 'SDKROOT'} & env.keys()
    assert env['LD_LIBRARY_PATH'] == str(tmp_path / 'lib')
    assert env['PATH'].split(os.pathsep)[0] == str(tmp_path / 'bin')


def test_hashes_ignore_timestamp_changes(tmp_path):
    path = tmp_path / 'header.h'
    path.write_text('identical content')
    prior = digest(path)
    os.utime(path, (1, 1))
    assert digest(path) == prior
