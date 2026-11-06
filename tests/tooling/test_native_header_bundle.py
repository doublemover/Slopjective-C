from pathlib import Path
import json
import os

import pytest

from scripts.objc3c_toolchain import native_headers as headers
from scripts.objc3c_toolchain.sdk_example import dependencies


def plan(tmp_path):
    source = tmp_path / 'inputs'
    source.mkdir()
    records, sources = {}, {}
    for name, text in {'roots/00/string': '// native string\n',
                       'roots/02/asm/types.h': '// native target types\n'}.items():
        path = source / Path(name).name
        path.write_text(text)
        records[name] = {'sha256': headers.digest(path), 'bytes': path.stat().st_size}
        sources[name] = str(path)
    manifest = {'schema': headers.SCHEMA, 'resource_version': '23',
                'roots': [{'index': 0, 'role': 'cxx'}, {'index': 1, 'role': 'resource'},
                          {'index': 2, 'role': 'externc'}], 'files': records}
    return {'sha256': headers.identity(manifest), 'manifest': manifest, 'sources': sources}


def test_configuration_preserves_native_order_classification_and_relocation(tmp_path):
    value = plan(tmp_path)
    config = headers.configuration(value['manifest'])
    assert '-nostdinc++' in config
    cpp = config.index('-cxx-isystem "<CFGDIR>/../lib/clang/23/objc3-native-headers/roots/00"')
    resource = config.index('-isystem "<CFGDIR>/../lib/clang/23/include"')
    platform = config.index('-Xclang -internal-externc-isystem')
    assert cpp < resource < platform
    assert str(tmp_path) not in config


def test_materialized_bundle_has_content_bound_inputs_and_no_source_symlinks(tmp_path):
    value = plan(tmp_path)
    source = Path(value['sources']['roots/02/asm/types.h'])
    alias = source.with_name('alias.h')
    alias.symlink_to(source)
    value['sources']['roots/02/asm/types.h'] = str(alias)
    prefix = tmp_path / 'installed sdk'
    report = headers.materialize(prefix, value)
    bundle = prefix / 'lib/clang/23' / headers.BUNDLE
    target = bundle / 'roots/02/asm/types.h'
    assert target.read_bytes() == source.read_bytes() and not target.is_symlink()
    assert report['files'] == 2 and report['sha256'] == value['sha256']
    assert json.loads((bundle / 'manifest.json').read_text()) == value['manifest']
    for name in ('clang.cfg', 'clang++.cfg'):
        assert (prefix / 'bin' / name).read_bytes() == (bundle / name).read_bytes()
    assert str(tmp_path / 'inputs') not in (bundle / 'manifest.json').read_text()
    assert not list((prefix / 'lib/clang/23').glob('.native-headers-*'))
    with pytest.raises(ValueError, match='already exists'):
        headers.materialize(prefix, value)


def test_same_size_source_edit_cannot_install_a_stale_header_plan(tmp_path):
    value = plan(tmp_path)
    source = Path(value['sources']['roots/00/string'])
    previous = source.stat()
    source.write_bytes(b'X' * previous.st_size)
    os.utime(source, ns=(previous.st_atime_ns, previous.st_mtime_ns))
    prefix = tmp_path / 'sdk'
    with pytest.raises(ValueError, match='input changed'):
        headers.materialize(prefix, value)
    assert not (prefix / 'lib/clang/23' / headers.BUNDLE).exists()
    assert not (prefix / 'bin/clang.cfg').exists()
    assert not list((prefix / 'lib/clang/23').glob('.native-headers-*'))


def test_header_plan_paths_and_resource_version_cannot_escape_the_prefix(tmp_path):
    value = plan(tmp_path)
    record = next(iter(value['manifest']['files'].values()))
    value['manifest']['files']['../outside.h'] = record
    value['sources']['../outside.h'] = next(iter(value['sources'].values()))
    value['sha256'] = headers.identity(value['manifest'])
    with pytest.raises(ValueError, match='escapes'):
        headers.materialize(tmp_path / 'sdk', value)
    value['manifest']['resource_version'] = '../../outside'
    value['sha256'] = headers.identity(value['manifest'])
    with pytest.raises(ValueError, match='resource directory version'):
        headers.materialize(tmp_path / 'another-sdk', value)
    assert not (tmp_path / 'outside.h').exists()


def test_dependency_spelling_preserves_search_roots_through_platform_symlinks(tmp_path):
    outside, include = tmp_path / 'kernel', tmp_path / 'include'
    outside.mkdir()
    include.mkdir()
    (outside / 'types.h').write_text('// kernel ABI\n')
    (include / 'asm').symlink_to(outside, target_is_directory=True)
    spelled = include / 'asm/types.h'
    depfile = tmp_path / 'headers.d'
    depfile.write_text(f'headers: {spelled}\n')
    assert dependencies(depfile, tmp_path) == [outside / 'types.h']
    assert dependencies(depfile, tmp_path, preserve_spelling=True) == [spelled]
