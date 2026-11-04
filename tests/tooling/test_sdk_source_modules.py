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


def test_installed_managed_example_retains_fresh_lookup_failure_after_temporary_build_cleanup(tmp_path, monkeypatch):
    import subprocess
    from scripts.objc3c_toolchain import qualify_source_modules
    sdk, output = tmp_path / 'sdk', tmp_path / 'evidence'
    sources = sdk / 'share/objc3c/examples/managed/src'
    sources.mkdir(parents=True)
    for name in ('provider.m', 'managed-value-support.h', 'main.m'):
        (sources / name).write_text('fixture input\n')
    builds = []
    def fail_build(argv, **kwargs):
        build = Path(argv[argv.index('--output-dir')+1])
        build.mkdir()
        builds.append(build)
        (build / 'lookup.log').write_text('fatal error: external header lookup failed at provider.m:7\n')
        raise subprocess.CalledProcessError(1, argv)
    monkeypatch.setattr(qualify_source_modules.subprocess, 'run', fail_build)
    with pytest.raises(subprocess.CalledProcessError):
        qualify_source_modules.qualify(sdk, output)
    assert builds and not builds[0].exists()
    assert 'provider.m:7' in (output / 'O0-cold-lookup.log').read_text()
