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
        identity = {'canonical_name': name, 'extension_abi': 'objc3-extension-abi-13:pinned',
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
        (directory / (name + '.objc3i.json')).write_text(json.dumps({'schema_id':'objc3c.semantic-interface.v2','schema_major':2,'ownership_projection':'','module': identity, 'dependencies': native_dependencies}))
        rows.append({'language': 'objective-c++', 'standard': 'c++20', 'ownership': 'arc', 'provider_ownership': 'arc',
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


def projected_bundle(bundle):
    root, manifest, _ = bundle
    leaf = manifest['packages'][0]
    leaf['provider_ownership'] = 'mrc'
    path = root / 'modules/fixture.Leaf/fixture.Leaf.objc3i.json'
    sidecar = json.loads(path.read_text())
    producer = copy.deepcopy(sidecar)
    producer['module']['canonical_name'] += '.__ownership_producer'
    producer['module']['runtime_abi'] = 'gnustep-2.2;manual-reference-counting'
    producer['module']['language_profile'] = producer['module']['language_profile'].replace(';arc=true;', ';arc=false;')
    producer['module']['semantic_fingerprint'] = 'c' * 64
    producer['source_binding_sha256'] = packages.digest(path.with_suffix(''))
    sidecar['source_binding_sha256'] = producer['source_binding_sha256']
    sidecar['ownership_projection'] = json.dumps(producer)
    sidecar['required_semantics'] = ['objc3.ownership-projection.v1']
    return root, manifest, leaf, path, sidecar, producer


def save_projection(root, manifest, row, path, sidecar):
    path.write_text(json.dumps(sidecar))
    row['artifacts'][path.relative_to(root).as_posix()] = packages.digest(path)
    (root / 'package.json').write_text(json.dumps(manifest))
    return packages.content_hash(manifest)


def test_source_free_install_preserves_opposite_mode_producer_proof(bundle, tmp_path):
    root, manifest, leaf, path, sidecar, _ = projected_bundle(bundle)
    expected = save_projection(root, manifest, leaf, path, sidecar)
    installed = packages.install(root, expected, tmp_path / 'store')
    actual = packages.verify_package(Path(installed['package']), expected)
    assert actual['packages'][0]['provider_ownership'] == 'mrc'
    assert actual['packages'][0]['ownership'] == 'arc'


@pytest.mark.parametrize('damage', ['producer-mode', 'package', 'nested', 'source', 'missing-capability',
    'missing-proof', 'unmarked-projection', 'language', 'standard', 'extension', 'old-schema', 'dependencies', 'nonobject-producer', 'nonobject-identity', 'nonarray-capabilities'])
def test_recomputed_package_identity_cannot_erase_projection_contract(bundle, tmp_path, damage):
    root, manifest, leaf, path, sidecar, producer = projected_bundle(bundle)
    if damage == 'nonobject-producer':
        producer = []
    elif damage == 'nonobject-identity':
        producer['module'] = []
    elif damage == 'nonarray-capabilities':
        sidecar['required_semantics'] = None
    elif damage == 'producer-mode':
        producer['module']['runtime_abi'] = 'gnustep-2.2;arc'
    elif damage == 'package':
        producer['module']['package_identity'] = 'wrong-package'
    elif damage == 'nested':
        producer['ownership_projection'] = 'nested'
    elif damage == 'source':
        producer['source_binding_sha256'] = 'd' * 64
    elif damage == 'missing-capability':
        sidecar['required_semantics'] = []
    elif damage == 'unmarked-projection':
        leaf['provider_ownership'] = 'arc'
    elif damage == 'language':
        producer['module']['language_profile'] = producer['module']['language_profile'].replace('objective-c++', 'objective-c')
    elif damage == 'standard':
        producer['module']['language_profile'] = producer['module']['language_profile'].replace('c++20', 'c++23')
    elif damage == 'extension':
        producer['module']['extension_abi'] = 'wrong-compiler'
    elif damage == 'old-schema':
        producer['schema_major'] = 1
    elif damage == 'dependencies':
        producer['dependencies'] = [{'forged': True}]
    sidecar['ownership_projection'] = '' if damage == 'missing-proof' else json.dumps(producer)
    expected = save_projection(root, manifest, leaf, path, sidecar)
    with pytest.raises(ValueError, match='projection|ownership'):
        packages.install(root, expected, tmp_path / 'rejected')
    assert not (tmp_path / 'rejected').exists()
