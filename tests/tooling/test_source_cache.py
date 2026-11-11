"""Pinned source acquisition must work offline after its first verified fetch."""
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import source_cache as owner


def git(path, *args):
    return subprocess.check_output(['git', '-C', str(path), *args], text=True,
                                   stderr=subprocess.DEVNULL).strip()


@pytest.fixture
def upstream(tmp_path):
    path = tmp_path / 'upstream';path.mkdir()
    git(path, 'init', '-q')
    git(path, 'config', 'user.name', 'Source cache control')
    git(path, 'config', 'user.email', 'cache@example.invalid')
    (path / 'source.c').write_text('int value = 42;\n')
    git(path, 'add', '.')
    git(path, 'commit', '-qm', 'source')
    git(path, 'tag', '-a', 'v1', '-m', 'pinned source')
    pin = {'url': str(path), 'tag': 'v1', 'commit': git(path, 'rev-parse', 'HEAD'),
           'tag_object': git(path, 'rev-parse', 'v1')}
    return path, pin


def test_cold_fetch_and_warm_checkout_need_no_remote_or_shared_object_database(tmp_path, upstream):
    remote, pin = upstream
    cache = tmp_path / 'cache.git'
    assert not owner.ensure(cache, **pin, fetch=True)['cache_hit']
    remote.rename(tmp_path / 'unavailable-upstream')
    destination = tmp_path / 'checkout'
    assert owner.checkout(cache, destination, **pin, fetch=True)['cache_hit']
    assert git(destination, 'rev-parse', 'HEAD') == pin['commit']
    assert not (destination / '.git/objects/info/alternates').exists()
    shutil.rmtree(cache)
    assert git(destination, 'show', 'HEAD:source.c') == 'int value = 42;'


@pytest.mark.parametrize('field', ['commit', 'tag_object'])
def test_wrong_pin_does_not_admit_or_replace_an_unowned_cache(tmp_path, upstream, field):
    _, pin = upstream
    cache = tmp_path / 'cache.git'
    owner.ensure(cache, **pin, fetch=True)
    (cache / 'objc3c-source-cache.json').unlink()
    changed = {**pin, field: '0' * 40}
    with pytest.raises(owner.SourceCacheError, match='pin|different commit'):
        owner.ensure(cache, **changed, fetch=True)
    assert git(cache, 'rev-parse', 'v1^{commit}') == pin['commit']


def test_cold_missing_cache_requires_fetch_authorization(tmp_path, upstream):
    _, pin = upstream
    with pytest.raises(owner.SourceCacheError, match='use --fetch'):
        owner.ensure(tmp_path / 'cache.git', **pin)


def test_changed_upstream_pin_replaces_only_owned_disposable_objects(tmp_path, upstream):
    remote, pin = upstream
    cache = tmp_path / 'cache.git'
    owner.ensure(cache, **pin, fetch=True)
    (remote / 'source.c').write_text('int value = 43;\n')
    git(remote, 'commit', '-qam', 'updated source')
    git(remote, 'tag', '-af', 'v1', '-m', 'new pinned source')
    changed = {**pin, 'commit': git(remote, 'rev-parse', 'HEAD'),
               'tag_object': git(remote, 'rev-parse', 'v1')}
    assert not owner.ensure(cache, **changed, fetch=True)['cache_hit']
    assert git(cache, 'rev-parse', 'v1^{commit}') == changed['commit']
    assert not any(path.name.startswith('.source-cache-') for path in tmp_path.iterdir())


def test_corrupt_blob_is_rejected_even_when_tag_and_commit_still_match(tmp_path, upstream):
    _, pin = upstream
    cache = tmp_path / 'cache.git'
    owner.ensure(cache, **pin, fetch=True)
    blob = git(cache, 'rev-parse', 'v1:source.c')
    object_path = cache / 'objects' / blob[:2] / blob[2:]
    object_path.parent.mkdir(exist_ok=True)
    if object_path.exists():
        object_path.chmod(0o600)
    object_path.write_bytes(b'corrupted Git object')
    with pytest.raises(owner.SourceCacheError):
        owner.ensure(cache, **pin)


def test_runtime_checkout_can_use_a_warm_cache_without_fetch(tmp_path, upstream):
    from scripts.objc3c_toolchain_runtime.build_linux import checkout
    remote, pin = upstream
    destination = tmp_path / 'toolchains/libobjc2-v1/src'
    cache = tmp_path / 'toolchains/source-cache/libobjc2-v1.git'
    owner.ensure(cache, **pin, fetch=True)
    remote.rename(tmp_path / 'unavailable-upstream')
    checkout(destination, pin['url'], pin['commit'], pin['tag'], False)
    assert git(destination, 'rev-parse', 'HEAD') == pin['commit']
