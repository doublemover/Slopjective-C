from __future__ import annotations

import copy
import json
from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain import binary_packages as packages


@pytest.fixture
def bundle(tmp_path):
    root = tmp_path / "binary package 'with spaces"
    root.mkdir()
    rows = []
    for name, dependencies in [('fixture.Leaf', []), ('fixture.Top', [{'name': 'fixture.Leaf', 'package': 'leaf-v1', 'visibility': 'public'}])]:
        identity = {'canonical_name': name, 'extension_abi': 'objc3-extension-abi-12:pinned',
                    'language_profile': 'objc3=enabled;arc=true;language=objective-c++;standard=c++20;blocks=true',
                    'language_revision': '202610', 'package_identity': 'leaf-v1' if name.endswith('Leaf') else 'top-v1',
                    'runtime_abi': 'gnustep-2.2;arc', 'semantic_fingerprint': 'a' * 64,
                    'target_triple': 'x86_64-unknown-linux-gnu'}
        directory = root / 'modules' / name
        directory.mkdir(parents=True)
        (directory / 'module.o').write_bytes(b'native-object')
        (directory / (name + '.objc3i')).write_text('int api(void);')
        native_dependencies = [{'module': next(row['module'] for row in rows if row['module']['canonical_name'] == edge['name']),
                                'public_import': edge['visibility'] == 'public',
                                'reexported': edge['visibility'] == 'public'} for edge in dependencies]
        (directory / (name + '.objc3i.json')).write_text(json.dumps({'module': identity, 'dependencies': native_dependencies}))
        rows.append({'language': 'objective-c++', 'standard': 'c++20', 'ownership': 'arc',
                     'module': identity, 'dependencies': dependencies,
                     'artifacts': {path.relative_to(root).as_posix(): packages.digest(path) for path in directory.iterdir()}})
    manifest = {'schema': packages.SCHEMA, 'sdk_seal': 'b' * 64, 'target': 'x86_64-unknown-linux-gnu',
                'optimization': 'O2', 'packages': rows}
    (root / 'package.json').write_text(json.dumps(manifest))
    return root, manifest, packages.content_hash(manifest)


def test_install_is_relocatable_idempotent_and_content_addressed(bundle, tmp_path):
    source, manifest, expected = bundle
    destination = tmp_path / 'installed'
    first = packages.install(source, expected, destination)
    second = packages.install(source, expected, destination)
    assert first == second
    assert Path(first['package']).name == expected
    moved = tmp_path / 'relocated'
    destination.rename(moved)
    assert packages.verify_package(moved / expected, expected) == manifest
    assert packages.closure(packages.package_rows(manifest), ['fixture.Top']) == ['fixture.Leaf', 'fixture.Top']
    assert packages.closure(packages.package_rows(manifest), ['fixture.Leaf']) == ['fixture.Leaf']


@pytest.mark.parametrize('damage', ['object', 'interface', 'sidecar', 'extra-file', 'symbolic-file', 'symbolic-directory', 'symbolic-root', 'wrong-lock'])
def test_install_rejects_damage_without_publishing(bundle, tmp_path, damage):
    root, manifest, expected = bundle
    directory = root / 'modules/fixture.Leaf'
    if damage in ('object', 'interface', 'sidecar'):
        name = {'object': 'module.o', 'interface': 'fixture.Leaf.objc3i', 'sidecar': 'fixture.Leaf.objc3i.json'}[damage]
        (directory / name).write_text('damaged')
    elif damage == 'extra-file':
        (root / 'unexpected').write_text('extra')
    elif damage == 'symbolic-file':
        path = directory / 'module.o'
        external = tmp_path / 'external'
        path.rename(external)
        path.symlink_to(external)
    elif damage == 'symbolic-directory':
        original = root / 'modules'
        external = tmp_path / 'external'
        original.rename(external)
        original.symlink_to(external, target_is_directory=True)
    elif damage == 'symbolic-root':
        alias = tmp_path / 'alias'
        alias.symlink_to(root, target_is_directory=True)
        root = alias
    else:
        expected = 'c' * 64
    store = tmp_path / 'installed'
    with pytest.raises(ValueError):
        packages.install(root, expected, store)
    assert not store.exists()


@pytest.mark.parametrize('damage', ['cycle', 'missing', 'package-lock', 'mode-lock', 'path', 'duplicate', 'unknown-field', 'sidecar-identity', 'visibility'])
def test_metadata_is_validated_even_with_recomputed_content_hash(bundle, tmp_path, damage):
    root, original, _ = bundle
    manifest = copy.deepcopy(original)
    leaf, top = manifest['packages']
    if damage == 'cycle':
        leaf['dependencies'] = [{'name': 'fixture.Top', 'package': 'top-v1', 'visibility': 'public'}]
    elif damage == 'missing':
        top['dependencies'][0]['name'] = 'fixture.Absent'
    elif damage == 'package-lock':
        top['dependencies'][0]['package'] = 'wrong'
    elif damage == 'mode-lock':
        leaf['standard'] = 'c++23'
        leaf['module']['language_profile'] = leaf['module']['language_profile'].replace('c++20', 'c++23')
    elif damage == 'path':
        leaf['artifacts']['../escaped.o'] = leaf['artifacts'].pop('modules/fixture.Leaf/module.o')
    elif damage == 'duplicate':
        manifest['packages'].append(leaf)
    elif damage == 'unknown-field':
        top['download'] = 'https://example.invalid'
    elif damage == 'visibility':
        top['dependencies'][0]['visibility'] = 'private'
    else:
        leaf['module']['semantic_fingerprint'] = 'd' * 64
    (root / 'package.json').write_text(json.dumps(manifest))
    with pytest.raises(ValueError):
        packages.install(root, packages.content_hash(manifest), tmp_path / 'installed')


def test_damaged_existing_installation_is_preserved_for_inspection(bundle, tmp_path):
    source, _, expected = bundle
    store = tmp_path / 'installed'
    installed = Path(packages.install(source, expected, store)['package'])
    obj = installed / 'modules/fixture.Leaf/module.o'
    obj.chmod(0o644)
    obj.write_bytes(b'corrupt installed object')
    with pytest.raises(ValueError, match='hash differs'):
        packages.install(source, expected, store)
    assert obj.read_bytes() == b'corrupt installed object'


def test_interrupted_copy_never_publishes_partial_package(bundle, tmp_path, monkeypatch):
    source, _, expected = bundle
    store = tmp_path / 'installed'
    def interrupted(*args, **kwargs):
        raise OSError('interrupted copy')
    monkeypatch.setattr(packages.shutil, 'copyfile', interrupted)
    with pytest.raises(OSError, match='interrupted'):
        packages.install(source, expected, store)
    assert not (store / expected).exists()
    assert list(store.iterdir()) == [store / '.package.lock']


def test_install_cannot_write_inside_its_source(bundle):
    root, _, expected = bundle
    with pytest.raises(ValueError, match='disjoint'):
        packages.install(root, expected, root / 'installed')


def test_consumer_wrong_sdk_rejects_before_output(bundle, tmp_path, monkeypatch):
    root, manifest, expected = bundle
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    app_root = tmp_path / 'app'
    app_root.mkdir()
    application = app_root / 'application.json'
    application.write_text('{}')
    monkeypatch.setattr(packages, 'sdk_identity', lambda _: {'seal_sha256': 'e' * 64, 'target': manifest['target']})
    output = tmp_path / 'consumer'
    with pytest.raises(ValueError, match='exact producing SDK'):
        packages.consume(sdk, root, expected, application, output)
    assert not output.exists()


def test_standalone_package_help_does_not_mutate_sdk(tmp_path):
    import os
    import subprocess
    import sys
    sources = Path(packages.__file__).parent
    for source, name in [('binary_packages.py', 'binary_packages.py'), ('source_graph.py', 'build.py'), ('sdk_example.py', 'sdk_example.py')]:
        shutil.copyfile(sources / source, tmp_path / name)
    env = dict(os.environ)
    env.pop('PYTHONDONTWRITEBYTECODE', None)
    result = subprocess.run([sys.executable, str(tmp_path / 'binary_packages.py'), '--help'], env=env, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stderr
    assert not (tmp_path / '__pycache__').exists()


def test_source_changes_during_install_are_rejected_before_publication(bundle, tmp_path, monkeypatch):
    root, _, expected = bundle
    copyfile = packages.shutil.copyfile
    source = root / 'modules/fixture.Leaf/module.o'
    def changing_copy(old, new):
        result = copyfile(old, new)
        if old == source:
            source.write_bytes(b'changed during copy')
        return result
    monkeypatch.setattr(packages.shutil, 'copyfile', changing_copy)
    store = tmp_path / 'installed'
    with pytest.raises(ValueError, match='hash differs'):
        packages.install(root, expected, store)
    assert not (store / expected).exists()


def test_failed_final_guard_does_not_publish_candidate(bundle, tmp_path):
    root, manifest, expected = bundle
    def failed_guard():
        raise ValueError('SDK changed during publication')
    store = tmp_path / 'installed'
    with pytest.raises(ValueError, match='SDK changed'):
        packages.copy_package(root, manifest, store, before_publish=failed_guard)
    assert not (store / expected).exists()
    assert list(store.iterdir()) == [store / '.package.lock']


def test_store_lock_rejects_concurrent_installation(bundle, tmp_path):
    root, _, expected = bundle
    store = tmp_path / 'installed'
    with packages.lock(store, '.package.lock'):
        with pytest.raises(BlockingIOError):
            packages.install(root, expected, store)
    assert not (store / expected).exists()


@pytest.mark.parametrize('damage', ['wrong-mode', 'wrong-package', 'missing-module'])
def test_consumer_invalid_dependency_rejects_before_compilation(bundle, tmp_path, monkeypatch, damage):
    root, manifest, expected = bundle
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    app = tmp_path / 'app'
    app.mkdir()
    (app / 'main.mm').write_text('int main() { return 0; }')
    config = {'schema': packages.CONSUMER, 'source': 'main.mm', 'language': 'objective-c++',
              'standard': 'c++20', 'ownership': 'arc',
              'dependencies': [{'name': 'fixture.Top', 'package': 'top-v1', 'visibility': 'private'}]}
    if damage == 'wrong-mode':
        config['standard'] = 'c++23'
    elif damage == 'wrong-package':
        config['dependencies'][0]['package'] = 'wrong'
    else:
        config['dependencies'][0]['name'] = 'fixture.Absent'
    application = app / 'application.json'
    application.write_text(json.dumps(config))
    monkeypatch.setattr(packages, 'sdk_identity', lambda _: {'seal_sha256': manifest['sdk_seal'], 'target': manifest['target']})
    output = tmp_path / 'consumer'
    with pytest.raises(ValueError, match='identity or mode lock'):
        packages.consume(sdk, root, expected, application, output)
    assert not output.exists()


def test_malformed_sidecar_shape_rejects_with_updated_hashes(bundle, tmp_path):
    root, manifest, _ = bundle
    sidecar = root / 'modules/fixture.Leaf/fixture.Leaf.objc3i.json'
    sidecar.write_text('[]')
    manifest['packages'][0]['artifacts'][sidecar.relative_to(root).as_posix()] = packages.digest(sidecar)
    (root / 'package.json').write_text(json.dumps(manifest))
    with pytest.raises(ValueError, match='sidecar differs'):
        packages.install(root, packages.content_hash(manifest), tmp_path / 'installed')
