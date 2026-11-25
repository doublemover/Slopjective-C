from __future__ import annotations

import copy
from functools import partial
from hashlib import sha256
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
import os
from pathlib import Path
import shutil
import stat
import subprocess
import sys
from threading import Thread
import zipfile

import pytest

from scripts.objc3c_toolchain import binary_packages as packages
from scripts.objc3c_toolchain import package_registry as registry


def test_installed_shared_library_accepts_one_versioned_soname_without_a_development_link(tmp_path):
    library = tmp_path / 'libLLVM.so.23.1'
    library.write_bytes(b'current LLVM')
    assert packages.installed_shared_library(tmp_path, 'libLLVM.so') == library

    (tmp_path / 'libLLVM.so.23').symlink_to(library.name)
    assert packages.installed_shared_library(tmp_path, 'libLLVM.so') == library


def test_installed_shared_library_rejects_ambiguous_missing_and_escaped_products(tmp_path):
    assert packages.find_installed_shared_library(tmp_path, 'libLLVM.so') is None
    with pytest.raises(ValueError, match='missing or ambiguous'):
        packages.installed_shared_library(tmp_path, 'libLLVM.so')
    for version in ('23.1', '24.0'):
        (tmp_path / ('libLLVM.so.' + version)).write_bytes(version.encode())
    with pytest.raises(ValueError, match='missing or ambiguous'):
        packages.installed_shared_library(tmp_path, 'libLLVM.so')
    outside = tmp_path.parent / 'libLLVM.so.25.0'
    outside.write_bytes(b'outside')
    (tmp_path / 'libLLVM.so').symlink_to(outside)
    with pytest.raises(ValueError, match='escapes'):
        packages.installed_shared_library(tmp_path, 'libLLVM.so')


def test_optional_installed_shared_library_rejects_broken_links(tmp_path):
    (tmp_path / 'libLLVM.so').symlink_to('missing.so.23')
    with pytest.raises(ValueError, match='broken link'):
        packages.find_installed_shared_library(tmp_path, 'libLLVM.so')


def package_cli(tmp_path, root_mode, command):
    repo = Path(__file__).resolve().parents[2]
    env = {**os.environ, 'PYTHONPATH': str(repo)}
    env.pop('OBJC3C_SDK_ROOT', None)
    selected = tmp_path / 'out/toolchains/linux-sdk/current'
    arguments = [sys.executable, '-B', '-m', 'scripts.objc3c_toolchain', 'packages']
    if root_mode == 'explicit':
        env['OBJC3C_SDK_ROOT'] = str(tmp_path / 'ignored-sdk')
        selected = tmp_path / 'explicit-sdk'
        arguments += ['--sdk-root', str(selected)]
    elif root_mode == 'environment':
        selected = tmp_path / 'environment-sdk'
        env['OBJC3C_SDK_ROOT'] = str(selected)
    selected.parent.mkdir(parents=True, exist_ok=True)
    return subprocess.run([*arguments, *map(str, command)], cwd=tmp_path,
                          env=env, capture_output=True, text=True, timeout=30), selected


@pytest.mark.parametrize('root_mode', ['explicit', 'environment', 'default'])
@pytest.mark.parametrize('command', ['publish', 'consume'])
def test_repository_cli_selects_sdk_with_controlled_diagnostics(tmp_path, root_mode, command):
    if command == 'publish':
        args = ['publish', '--build-dir', tmp_path / 'build', '--store', tmp_path / 'store']
    else:
        args = ['consume', '--package', tmp_path / 'package', '--content-sha256', 'a' * 64,
                '--application', tmp_path / 'app.json', '--output-dir', tmp_path / 'output']
    result, selected = package_cli(tmp_path, root_mode, args)
    assert result.returncode == 1
    assert f'binary package {command} failed:' in result.stderr
    assert str(selected) in result.stderr
    assert 'Traceback' not in result.stderr and 'NameError' not in result.stderr


@pytest.mark.parametrize('root_mode', ['explicit', 'environment', 'default'])
def test_repository_cli_installs_without_sdk(bundle, tmp_path, root_mode):
    source, manifest, expected = bundle
    result, _ = package_cli(tmp_path, root_mode, [
        'install', '--package', source, '--content-sha256', expected,
        '--store', tmp_path / 'installed'])
    assert result.returncode == 0, result.stderr
    report = json.loads(result.stdout)
    assert report['status'] == 'PASS'
    assert packages.verify_package(Path(report['package']), expected) == manifest


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
        (directory / (name + '.objc3i.json')).write_text(json.dumps({'schema_id':'objc3c.semantic-interface.v2','schema_major':2,'schema_minor':2,'declarations':[{'kind':'Function','qualified_name':'api','export_root':'api'}],'ownership_projection':'','module': identity, 'dependencies': native_dependencies}))
        rows.append({'language': 'objective-c++', 'standard': 'c++20', 'ownership': 'arc', 'provider_ownership': 'arc', 'exports': ['*'],
                     'module': identity, 'dependencies': dependencies,
                     'artifacts': {path.relative_to(root).as_posix(): packages.digest(path) for path in directory.iterdir()}})
    manifest = {'schema': packages.SCHEMA, 'sdk_seal': 'b' * 64, 'target': 'x86_64-unknown-linux-gnu',
                'optimization': 'O2', 'packages': rows}
    (root / 'package.json').write_text(json.dumps(manifest))
    return root, manifest, packages.content_hash(manifest)


@pytest.mark.parametrize('damage', [None, 'missing', 'extra', 'duplicate', 'empty', 'old-schema'])
def test_selected_exports_bind_native_public_roots(bundle, tmp_path, damage):
    source, manifest, _ = bundle
    for row in manifest['packages']:
        row['exports'] = ['Answer', 'answer']
        name = row['module']['canonical_name']
        path = source / f'modules/{name}/{name}.objc3i.json'
        sidecar = json.loads(path.read_text())
        sidecar['declarations'] = [{'kind': 'Typedef', 'qualified_name': 'Answer', 'export_root': 'Answer'},
                                   {'kind': 'Function', 'qualified_name': 'answer', 'export_root': 'answer'},
                                   {'kind': 'Field', 'qualified_name': 'member', 'export_root': 'Answer'}]
        path.write_text(json.dumps(sidecar))
        row['artifacts'][str(path.relative_to(source))] = packages.digest(path)
    row = manifest['packages'][0]
    if damage == 'missing': row['exports'] = ['answer']
    elif damage == 'extra': row['exports'].append('hidden')
    elif damage == 'duplicate': row['exports'].append('answer')
    elif damage == 'empty': row['exports'] = []
    elif damage == 'old-schema': manifest['schema'] = 'objc3c.binary-package.v2'
    (source / 'package.json').write_text(json.dumps(manifest))
    identity = packages.content_hash(manifest)
    if damage:
        with pytest.raises(ValueError, match='exports|schema'):
            packages.install(source, identity, tmp_path / 'store')
    else:
        assert packages.install(source, identity, tmp_path / 'store')['status'] == 'PASS'


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


def test_archive_and_inspection_are_reproducible_and_need_no_sdk(bundle, tmp_path):
    source, manifest, identity = bundle
    directory = tmp_path / 'relocated source'
    shutil.copytree(source, directory)
    for path in directory.rglob('*'):
        if path.is_file():
            path.chmod(0o600)
            os.utime(path, (0, 0))
    (directory / 'package.json').write_bytes(packages.canonical_json(manifest))

    archive, replay = tmp_path / 'published.zip', tmp_path / 'replayed.zip'
    result, _ = package_cli(tmp_path, 'default', [
        'export', '--package', source, '--content-sha256', identity, '--output', archive])
    assert result.returncode == 0, result.stderr
    assert json.loads(result.stdout)['archive_sha256'] == packages.digest(archive)
    packages.export_archive(directory, identity, replay)
    assert archive.read_bytes() == replay.read_bytes()
    with zipfile.ZipFile(archive) as package:
        assert package.namelist() == ['package.json', *sorted(packages.artifacts(manifest))]
        assert all(member.date_time == (1980, 1, 1, 0, 0, 0) and
                   member.external_attr >> 16 == stat.S_IFREG | 0o644
                   for member in package.infolist())

    outputs = []
    for path in (source, archive):
        result, _ = package_cli(tmp_path, 'default', [
            'inspect', '--package', path, '--content-sha256', identity, '--module', 'fixture.Top', 'top-v1'])
        assert result.returncode == 0, result.stderr
        outputs.append(result.stdout)
    assert outputs[0] == outputs[1]
    inspected = json.loads(outputs[0])
    assert inspected['roots'] == ['fixture.Top']
    assert inspected['modules'] == manifest['packages']
    with pytest.raises(ValueError, match='identity lock'):
        packages.inspect(archive, identity, [('fixture.Top', 'wrong-v1')])

    store = tmp_path / 'installed'
    result, _ = package_cli(tmp_path, 'default', [
        'install', '--package', archive, '--content-sha256', identity, '--store', store])
    assert result.returncode == 0, result.stderr
    assert json.loads(result.stdout) == packages.install(archive, identity, store)
    assert packages.verify_package(store / identity, identity) == manifest
    assert all(stat.S_IMODE(path.stat().st_mode) == 0o444
               for path in (store / identity).rglob('*') if path.is_file())


@pytest.mark.parametrize('defect,diagnostic', [
    ('missing', 'missing or unlisted'), ('extra', 'missing or unlisted'),
    ('duplicate', 'duplicate'), ('link', 'regular-file'), ('changed', 'artifact hash'),
    ('sidecar', 'sidecar differs'), ('lock', 'content identity'),
])
def test_archive_admission_rejects_invalid_members_without_publishing(bundle, tmp_path, defect, diagnostic):
    source, manifest, identity = bundle
    contents = {name: (source / name).read_bytes() for name in packages.artifacts(manifest)}
    object_name = 'modules/fixture.Top/module.o'
    sidecar_name = 'modules/fixture.Top/fixture.Top.objc3i.json'
    if defect == 'missing':
        contents.pop(object_name)
    elif defect == 'extra':
        contents['../unexpected.o'] = b'unlisted'
    elif defect == 'changed':
        contents[object_name] += b'changed bytes'
    elif defect == 'sidecar':
        sidecar = json.loads(contents[sidecar_name])
        sidecar['module']['package_identity'] = 'other-v1'
        contents[sidecar_name] = json.dumps(sidecar).encode()
        manifest['packages'][1]['artifacts'][sidecar_name] = sha256(contents[sidecar_name]).hexdigest()
        identity = packages.content_hash(manifest)
    elif defect == 'lock':
        identity = '0' * 64
    archive = tmp_path / 'rejected.zip'
    with zipfile.ZipFile(archive, 'w') as package:
        package.writestr('package.json', packages.canonical_json(manifest))
        for name, content in contents.items():
            member = zipfile.ZipInfo(name)
            if name == object_name and defect == 'link':
                member.external_attr = (stat.S_IFLNK | 0o777) << 16
            package.writestr(member, content)
        if defect == 'duplicate':
            with pytest.warns(UserWarning, match='Duplicate name'):
                package.writestr(object_name, contents[object_name])
    store = tmp_path / 'installed'
    with pytest.raises(ValueError, match=diagnostic):
        packages.install(archive, identity, store)
    assert sorted(path.name for path in store.iterdir()) == ['.package.lock']
    assert not (tmp_path / 'unexpected.o').exists()
    with pytest.raises(ValueError, match=diagnostic):
        packages.inspect(archive, identity, [])


def test_truncated_archive_is_rejected_by_cli(bundle, tmp_path):
    source, _, identity = bundle
    archive = tmp_path / 'truncated.zip'
    packages.export_archive(source, identity, archive)
    archive.write_bytes(archive.read_bytes()[:-22])
    result, _ = package_cli(tmp_path, 'default', [
        'install', '--package', archive, '--content-sha256', identity, '--store', tmp_path / 'installed'])
    assert result.returncode == 1
    assert 'binary package install failed:' in result.stderr and 'Traceback' not in result.stderr
    assert not (tmp_path / 'installed' / identity).exists()


def test_hosted_registry_resolves_pinned_media_and_replays_offline(bundle, tmp_path, monkeypatch):
    source, manifest, identity = bundle
    hosted = tmp_path / 'hosted'
    publication = registry.publish(source, identity, hosted)
    assert registry.publish(source, identity, hosted) == publication
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    monkeypatch.setattr(packages, 'sdk_identity',
                        lambda _: {'seal_sha256': manifest['sdk_seal'], 'target': manifest['target']})
    store = tmp_path / 'installed'
    modules = [('fixture.Top', 'top-v1')]
    handler = partial(SimpleHTTPRequestHandler, directory=hosted)
    with ThreadingHTTPServer(('127.0.0.1', 0), handler) as server:
        worker = Thread(target=server.serve_forever)
        worker.start()
        try:
            url = f'http://127.0.0.1:{server.server_port}/index.json'
            pin = publication['index_sha256']
            with pytest.raises(ValueError, match='index SHA-256'):
                registry.fetch(sdk, url, '0' * 64, identity, modules, store)
            with pytest.raises(ValueError, match='module identity locks'):
                registry.fetch(sdk, url, pin, identity, [('fixture.Top', 'wrong')], store)
            archive = Path(publication['archive'])
            contents = archive.read_bytes()
            archive.write_bytes(contents[:-1])
            with pytest.raises(ValueError, match='size or SHA-256'):
                registry.fetch(sdk, url, pin, identity, modules, store)
            assert not (store / identity).exists()
            archive.write_bytes(contents)
            first = registry.fetch(sdk, url, pin, identity, modules, store)
        finally:
            server.shutdown()
            worker.join()
    assert not first['cache_hit']
    assert registry.fetch(sdk, url, pin, identity, modules, store, offline=True) == {**first, 'cache_hit': True}
    with pytest.raises(ValueError, match='pinned index in the cache'):
        registry.fetch(sdk, url, pin, identity, modules, tmp_path / 'empty', offline=True)


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


@pytest.mark.parametrize('damage,diagnostic', [
    ('object', 'conflicting immutable providers for module: fixture.Leaf'),
    ('visibility', 'conflicting immutable providers for module: fixture.Top'),
    ('optimization', 'same optimization'), ('sdk', 'exact producing SDK'),
])
def test_consumer_rejects_incompatible_package_sets_before_compilation(bundle, tmp_path, monkeypatch,
                                                                      damage, diagnostic):
    root, original, identity = bundle
    extra = tmp_path / 'extra'
    shutil.copytree(root, extra)
    manifest = copy.deepcopy(original)
    if damage == 'object':
        path = 'modules/fixture.Leaf/module.o'
        (extra / path).write_bytes(b'different provider definition')
        manifest['packages'][0]['artifacts'][path] = packages.digest(extra / path)
    elif damage == 'visibility':
        row = manifest['packages'][1]
        row['dependencies'][0]['visibility'] = 'private'
        path = 'modules/fixture.Top/fixture.Top.objc3i.json'
        sidecar = json.loads((extra / path).read_text())
        sidecar['dependencies'][0].update(public_import=False, reexported=False)
        (extra / path).write_text(json.dumps(sidecar))
        row['artifacts'][path] = packages.digest(extra / path)
    elif damage == 'optimization':
        manifest['optimization'] = 'O0'
    else:
        manifest['sdk_seal'] = 'c' * 64
    (extra / 'package.json').write_text(json.dumps(manifest))
    extra_identity = packages.content_hash(manifest)
    assert packages.verify_package(extra, extra_identity) == manifest

    sdk, client = tmp_path / 'sdk', tmp_path / 'client'
    sdk.mkdir()
    client.mkdir()
    app = client / 'application.json'
    app.write_text('{}')
    output = tmp_path / 'consumer'
    monkeypatch.setattr(packages, 'sdk_identity',
                        lambda _: {'seal_sha256': original['sdk_seal'], 'target': original['target']})
    with pytest.raises(ValueError, match=diagnostic):
        packages.consume(sdk, root, identity, app, output, additional=((extra, extra_identity),))
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


@pytest.mark.parametrize('damage', [None, 'omitted-namespace', 'unselected-root',
                                   'missing-owner', 'missing-contract', 'old-interface'])
def test_scoped_native_export_roots(bundle, tmp_path, damage):
    source, manifest, _ = bundle
    for row in manifest['packages']:
        row['exports'] = ['@using-enum:Api::Kind', '@using:Api', 'Api', 'Short']
        name = row['module']['canonical_name']
        path = source / f'modules/{name}/{name}.objc3i.json'
        sidecar = json.loads(path.read_text())
        sidecar['declarations'] = [
            {'kind': kind, 'qualified_name': name, 'export_root': root}
            for kind, name, root in [
                ('Namespace', 'Api', 'Api'),
                ('CXXRecord', 'Api::Box', 'Api::Box'),
                ('Var', 'Api::Box::counter', 'Api::Box'),
                ('Function', 'hidden_friend', 'Api::Box'),
                ('Namespace', 'Api::v1', 'Api::v1'),
                ('Function', 'Api::v1::value', 'Api::v1::value'),
                ('Enum', 'Api::Kind', 'Api::Kind'),
                ('Using', 'Api::alias', 'Api::alias'),
                ('NamespaceAlias', 'Short', 'Short'),
                ('UsingDirective', '@using:Api', '@using:Api'),
                ('UsingEnum', '@using-enum:Api::Kind', '@using-enum:Api::Kind'),
            ]]
        if damage == 'omitted-namespace': row['exports'].remove('Api')
        elif damage == 'unselected-root':
            sidecar['declarations'].append(
                {'kind': 'Function', 'qualified_name': 'hidden', 'export_root': 'hidden'})
        elif damage == 'missing-owner': sidecar['declarations'][3]['export_root'] = 'Absent'
        elif damage == 'missing-contract': del sidecar['declarations'][3]['export_root']
        elif damage == 'old-interface': sidecar['schema_minor'] = 1
        path.write_text(json.dumps(sidecar))
        row['artifacts'][str(path.relative_to(source))] = packages.digest(path)
    (source / 'package.json').write_text(json.dumps(manifest))
    identity = packages.content_hash(manifest)
    if damage:
        with pytest.raises(ValueError, match='exports'):
            packages.install(source, identity, tmp_path / 'store')
    else:
        assert packages.install(source, identity, tmp_path / 'store')['status'] == 'PASS'


def test_namespace_ancestors_do_not_expand_leaf_export_policy(bundle, tmp_path):
    source, manifest, _ = bundle
    for row in manifest['packages']:
        row['exports'] = ['Api::value']
        path = source / f"modules/{row['module']['canonical_name']}/{row['module']['canonical_name']}.objc3i.json"
        sidecar = json.loads(path.read_text())
        sidecar['declarations'] = [
            {'kind': 'Namespace', 'qualified_name': 'Api', 'export_root': 'Api'},
            {'kind': 'Function', 'qualified_name': 'Api::value', 'export_root': 'Api::value'}]
        path.write_text(json.dumps(sidecar))
        row['artifacts'][str(path.relative_to(source))] = packages.digest(path)
    (source / 'package.json').write_text(json.dumps(manifest))
    assert packages.install(source, packages.content_hash(manifest), tmp_path / 'store')['status'] == 'PASS'
