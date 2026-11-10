"""Independent Swift admission, exact header use and anti-green controls."""
import copy
import errno
import hashlib
import json
import os
from pathlib import Path
import shutil
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import swift_consumer as owner
from scripts.objc3c_toolchain import swift_toolchain as tools

REPO = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('mutation', ['missing', 'duplicate', 'unknown', 'optional', 'declaration', 'boundary'])
def test_required_rows_cannot_disappear_or_be_promoted_without_a_contract(tmp_path, mutation):
    value = json.loads((REPO / 'tests/foreign/swift_consumer/rows.json').read_text())
    if mutation == 'missing': value['rows'].pop()
    elif mutation == 'duplicate': value['rows'][-1] = value['rows'][0]
    elif mutation == 'unknown': value['rows'][0]['id'] = 'invented'
    elif mutation == 'optional': value['rows'][0]['required'] = False
    elif mutation == 'declaration': value['rows'][0]['declaration'] = 'different_symbol'
    else: value['unqualified'][0]['status'] = 'swift-call-qualified'
    path = tmp_path / 'rows.json';path.write_text(json.dumps(value))
    with pytest.raises(ValueError, match='inventory|boundaries'):
        owner.row_manifest(path)


@pytest.mark.parametrize('mutation', ['wrong', 'bool', 'missing', 'empty', 'malformed'])
def test_mixed_language_oracle_requires_exact_values_and_lifetime_counts(mutation):
    value = dict(owner.EXPECTED)
    if mutation == 'wrong': value['live'] = 1
    elif mutation == 'bool': value['live'] = False
    elif mutation == 'missing': del value['read']
    elif mutation == 'empty': value = {}
    text = 'not JSON' if mutation == 'malformed' else json.dumps(value)
    with pytest.raises(ValueError, match='oracle|contract'):
        owner.execution_result(text)


@pytest.mark.parametrize('missing', ['header', 'module-map'])
def test_successful_import_must_name_the_actual_selected_header(tmp_path, missing):
    header, module = tmp_path / 'API.h', tmp_path / 'module.modulemap'
    header.write_text('declaration');module.write_text('module')
    depfile = tmp_path / 'consumer.d'
    depfile.write_text('consumer: '+str(module if missing == 'header' else header)+'\n')
    with pytest.raises(ValueError, match='exact selected header'):
        owner.import_evidence(depfile, tmp_path, header, module)
    depfile.write_text('consumer: '+str(module)+' '+str(header)+'\n')
    assert set(owner.import_evidence(depfile, tmp_path, header, module)) == {str(header), str(module)}


def test_foreign_environment_excludes_product_loader_and_importer_overrides(monkeypatch, tmp_path):
    for key in ('LD_LIBRARY_PATH', 'SDKROOT', 'SWIFT_EXEC', 'CPATH', 'LIBRARY_PATH'):
        monkeypatch.setenv(key, '/incorrect/product')
    result = tools.environment(tmp_path)
    assert not set(result) & {'LD_LIBRARY_PATH', 'SDKROOT', 'SWIFT_EXEC', 'CPATH', 'LIBRARY_PATH'}
    assert result['PATH'].startswith(str(tmp_path / 'usr/bin'))


@pytest.mark.parametrize('system,machine,distribution,version', [
    ('Darwin', 'x86_64', 'debian', '13'), ('Linux', 'aarch64', 'debian', '13'),
    ('Linux', 'x86_64', 'ubuntu', '26.04'), ('Linux', 'x86_64', 'debian', '12')])
def test_unadmitted_host_is_not_a_semantic_pass(monkeypatch, system, machine, distribution, version):
    monkeypatch.setattr(tools.platform, 'system', lambda: system)
    monkeypatch.setattr(tools.platform, 'machine', lambda: machine)
    monkeypatch.setattr(tools.platform, 'freedesktop_os_release', lambda: {'ID': distribution, 'VERSION_ID': version})
    with pytest.raises(ValueError, match='admitted Debian'):
        tools.require_host()


@pytest.mark.parametrize('name', ['/absolute', '../escape', 'different-release/bin/swift'])
def test_archive_selection_rejects_escaped_or_wrong_release_paths(name):
    pin, _ = tools.policy(REPO)
    with pytest.raises(ValueError, match='escaped|release root'):
        tools.wanted(name, pin)


def test_selected_toolchain_excludes_unused_development_payloads():
    pin, _ = tools.policy(REPO)
    root = pin['archive_root']+'/'
    for name in ('usr/bin/lldb', 'usr/bin/swift-package', 'usr/lib/swift/pm/library',
                 'usr/lib/swift/embedded/library', 'usr/lib/swift/migrator/library'):
        assert tools.wanted(root+name, pin) is None
    for name in ('usr/bin/swiftc', 'usr/bin/clang-21', 'usr/bin/lld', 'usr/lib/swift/linux/libswiftCore.so'):
        assert tools.wanted(root+name, pin) == name


def test_policy_identity_is_stable_under_json_formatting(tmp_path):
    root = tmp_path / 'toolchains/swift';root.mkdir(parents=True)
    shutil.copyfile(REPO / 'toolchains/swift/signing-key.asc', root / 'signing-key.asc')
    value = json.loads((REPO / 'toolchains/swift/manifest.json').read_text())
    (root / 'manifest.json').write_text(json.dumps(value, sort_keys=True))
    assert tools.policy(tmp_path) == tools.policy(REPO)


@pytest.mark.parametrize('mutation', ['archive-size', 'archive-bytes', 'signature', 'gpg-error', 'signer'])
def test_release_admission_requires_archive_signature_and_exact_signing_fingerprint(tmp_path, monkeypatch, mutation):
    archive, signature = tmp_path / 'swift.tar.gz', tmp_path / 'swift.sig'
    archive.write_bytes(b'archive');signature.write_bytes(b'signature')
    pin = {'archive_bytes': archive.stat().st_size, 'archive_sha256': tools.digest(archive),
        'signature_sha256': tools.digest(signature), 'public_key': 'public-key.asc',
        'signing_fingerprint': 'a'*40}
    if mutation == 'archive-size': archive.write_bytes(b'changed length')
    elif mutation == 'archive-bytes': archive.write_bytes(b'changed')
    elif mutation == 'signature': signature.write_bytes(b'changed')
    monkeypatch.setattr(tools.shutil, 'which', lambda *args, **kwargs: '/bin/true')
    monkeypatch.setattr(tools.subprocess, 'run', lambda *args, **kwargs: SimpleNamespace(
        returncode=int(mutation == 'gpg-error'), stdout='[GNUPG:] VALIDSIG '+'b'*40+' ', stderr=''))
    work = tmp_path / 'work';work.mkdir()
    with pytest.raises(ValueError, match='version pin|verification failed|fingerprint'):
        tools.verify_archive(tmp_path, archive, signature, work, pin)


@pytest.mark.parametrize('mutation', ['bytes', 'mode', 'link', 'extra', 'rewritten-seal'])
def test_local_swift_seal_cannot_hide_changed_publisher_files(tmp_path, monkeypatch, mutation):
    monkeypatch.setattr(tools, 'require_host', lambda: {})
    root = tmp_path / 'swift';root.mkdir()
    binary = root / 'swiftc';binary.write_bytes(b'publisher bytes');binary.chmod(0o755)
    (root / 'swift').symlink_to('swiftc')
    files = {'swiftc': {'sha256': tools.digest(binary), 'mode': 0o755}, 'swift': {'link': 'swiftc'}}
    pin = {'archive_sha256': 'a'*64, 'selected_manifest_sha256': hashlib.sha256(
        json.dumps(files, sort_keys=True, separators=(',', ':')).encode()).hexdigest()}
    monkeypatch.setattr(tools, 'policy', lambda repo: (pin, 'pinned-policy'))
    receipt = {'schema': 'objc3c.swift-consumer-install.v1', 'pin_sha256': 'pinned-policy',
               'archive_sha256': 'a'*64, 'files': copy.deepcopy(files)}
    seal = root / 'objc3-swift-consumer.json';seal.write_text(json.dumps(receipt))
    assert tools.verify(tmp_path, root)['files'] == files
    if mutation in ('bytes', 'rewritten-seal'):
        binary.write_bytes(b'substituted binary')
        if mutation == 'rewritten-seal':
            receipt['files']['swiftc']['sha256'] = tools.digest(binary)
            seal.write_text(json.dumps(receipt))
    elif mutation == 'mode': binary.chmod(0o644)
    elif mutation == 'extra': (root / 'surprise').touch()
    else:
        (root / 'swift').unlink();(root / 'swift').symlink_to('/bin/sh')
    with pytest.raises(ValueError, match='changed|escaped|publisher pin'):
        tools.verify(tmp_path, root)


@pytest.mark.parametrize('mutation', [None, 'sdk', 'swift', 'oracle', 'dependencies', 'unexpected-error'])
def test_complete_consumer_protocol_fails_closed_at_real_admission_boundaries(tmp_path, monkeypatch, mutation):
    repo, sdk, swift = (tmp_path / name for name in ('repo', 'sdk', 'swift'))
    fixture = repo / 'tests/foreign/swift_consumer'
    shutil.copytree(REPO / 'tests/foreign/swift_consumer', fixture)
    for root in (sdk, swift): root.mkdir()
    (sdk / 'lib').mkdir()
    (swift / 'objc3-swift-consumer.json').write_text('test-only seal')
    symbolizer = tmp_path / 'llvm-symbolizer';symbolizer.touch()
    calls = []
    def verify_sdk(root):
        calls.append(root)
        return {'seal_sha256': ('b' if mutation == 'sdk' and len(calls) > 1 else 'a')*64, 'source_build_id': 'test-only'}
    swift_calls = []
    def verify_swift(repo, root):
        swift_calls.append(root)
        if mutation == 'swift' and len(swift_calls) > 1: raise ValueError('Swift seal changed')
        return {'archive_sha256': 'c'*64}
    monkeypatch.setattr(owner, 'verify_sdk', verify_sdk)
    monkeypatch.setattr(owner, 'verify_swift', verify_swift)
    monkeypatch.setattr(owner, 'require_host', lambda: {'host': 'test-only'})
    monkeypatch.setattr(owner, 'loaded_graph', lambda *args, **kwargs: {'libraries': {}})
    def command(argv, *, cwd, environment, log, timeout):
        argv = list(map(str, argv));name = log.stem
        status, text = 0, ''
        if mutation == 'unexpected-error' and name == 'swift-version': raise KeyError('deliberate protocol error')
        if name == 'swift-version': text = 'Swift version 6.4.2 (swift-6.4.2-RELEASE)'
        elif name == 'swift-target-info': text = json.dumps({'target': {'triple': 'x86_64-unknown-linux-gnu'},
            'paths': {'runtimeResourcePath': str(swift / 'usr/lib/swift')}})
        elif name == 'native-symbolizer-version': text = 'LLVM version 23.1.5'
        elif '--emit-interface' in argv:
            source, destination = Path(argv[1]), Path(argv[argv.index('--emit-interface')+1])
            destination.write_bytes(source.read_bytes())
            destination.with_suffix(destination.suffix+'.json').write_text(json.dumps({'module': {
                'semantic_fingerprint': tools.digest(source)}}))
        elif name.startswith('negative-'):
            status, text = 1, owner.NEGATIVES[name.removeprefix('negative-')][1]
        elif name == 'changed-name-control': status, text = 1, "cannot find 'projectedValue'"
        elif name == 'missing-provider-control': status, text = 1, 'objc3_swift_value'
        elif '-typecheck' in argv:
            module = Path(argv[argv.index('-I')+1]);depfile = Path(argv[argv.index('-emit-dependencies-path')+1])
            inputs = [module / 'module.modulemap']
            if mutation != 'dependencies': inputs.append(module / 'API.h')
            depfile.write_text('consumer: '+ ' '.join(str(path).replace(' ', '\\ ') for path in inputs)+'\n')
        elif '-o' in argv: Path(argv[argv.index('-o')+1]).write_bytes(b'test-only object or binary')
        elif name.endswith('-execute'):
            text = json.dumps({**owner.EXPECTED, **({'live': 1} if mutation == 'oracle' else {})})
        log.write_text(text)
        return {'argv': argv, 'exit_code': status, 'timed_out': False, 'log': str(log), 'log_sha256': tools.digest(log)}
    monkeypatch.setattr(owner, 'command', command)
    # Avoid creating irrelevant fake cache contents; removal still executes.
    original_rmtree = shutil.rmtree
    monkeypatch.setattr(owner.shutil, 'rmtree', lambda path: original_rmtree(path) if path.exists() else None)
    output = repo / 'out/consumer'
    if mutation:
        with pytest.raises((ValueError, KeyError)):
            owner.qualify(repo, output, sdk=sdk, swift_root=swift, symbolizer=symbolizer)
        assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'
    else:
        report = owner.qualify(repo, output, sdk=sdk, swift_root=swift, symbolizer=symbolizer)
        assert report['status'] == 'PASS' and len(report['cases']) == len(report['controls']) == 8
        assert report['relocation'] == {'original_header_hidden': True, 'provider_source_hidden': True,
                                      'replayed_header_unchanged': True}
        assert {row['status'] for row in report['rows']} == {'swift-call-qualified', 'swift-import-qualified'}
        from scripts.objc3c_assurance.evidence import suite_results
        native_root = repo / 'out/native'
        destination = native_root / 'swift-consumer/consumer'
        destination.parent.mkdir(parents=True)
        shutil.move(output, destination)
        assert len(suite_results(native_root, 'swift-consumer')) == 8
        for key in ('cases', 'controls', 'rows', 'relocation'):
            broken = copy.deepcopy(report)
            if key == 'relocation': broken[key] = {}
            else: broken[key].pop()
            (destination / 'qualification.json').write_text(json.dumps(broken))
            with pytest.raises(ValueError, match='incomplete'):
                suite_results(native_root, 'swift-consumer')


@pytest.mark.parametrize('path', ['native/objc3c/src/clang_foundation/interface_emitter.cpp',
    'native/objc3c/src/clang_foundation/interface_contract.h', 'toolchains/swift/manifest.json',
    'tests/foreign/swift_consumer/api.h', 'toolchains/llvm/patches/change.patch',
    'scripts/objc3c_toolchain/swift_consumer.py', 'scripts/objc3c_assurance/runner.py'])
def test_foreign_contract_changes_require_the_existing_native_ci_consumer(path):
    from scripts.objc3c_assurance.plan import select
    revision = dict(head='a'*40, base='b'*40, tested='c'*40)
    assert 'swift-consumer' in {suite['id'] for suite in select([path], **revision)['suites']}
    assert 'swift-consumer' in {suite['id'] for suite in select(['docs/guide.md'], **revision, periodic=True)['suites']}
    assert 'swift-consumer' not in {suite['id'] for suite in select(['stdlib/ObjC3Map.h'], **revision)['suites']}


def test_development_pilot_prefix_seals_only_current_native_tools_without_an_archive(tmp_path):
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/objc3c', 'lib/libLLVM.so.23.1', 'lib/libclang-cpp.so.23.1',
                     'lib/clang/23/include/stddef.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    value = owner.prepare_compiler_prefix(stage, prefix, source_build_id='current-build',
                                           target='x86_64-unknown-linux-gnu')
    assert value['source_build_id'] == 'current-build'
    assert (prefix / 'bin/clang').stat().st_ino == (stage / 'bin/clang').stat().st_ino
    assert (prefix / 'lib/libLLVM.so').resolve() == prefix / 'lib/libLLVM.so.23.1'
    assert (prefix / 'lib/libclang-cpp.so').resolve() == prefix / 'lib/libclang-cpp.so.23.1'
    with pytest.raises(ValueError, match='fresh independent'):
        owner.prepare_compiler_prefix(stage, prefix, source_build_id='current-build', target='target')


def test_development_pilot_prefix_accepts_a_static_compiler_stage(tmp_path):
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/objc3c', 'lib/clang/23/include/stddef.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    value = owner.prepare_compiler_prefix(stage, prefix, source_build_id='static-build',
                                           target='x86_64-unknown-linux-gnu')
    assert value['source_build_id'] == 'static-build'
    assert not (prefix / 'lib/libLLVM.so').exists()
    assert not (prefix / 'lib/libclang-cpp.so').exists()


def test_development_pilot_prefix_copies_across_filesystems(tmp_path, monkeypatch):
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/objc3c', 'lib/clang/23/include/stddef.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    monkeypatch.setattr(owner.os, 'link', lambda *_: (_ for _ in ()).throw(OSError(errno.EXDEV, 'cross-device')))
    value = owner.prepare_compiler_prefix(stage, prefix, source_build_id='copied-build',
                                           target='x86_64-unknown-linux-gnu')
    assert value['source_build_id'] == 'copied-build'
    assert (prefix / 'bin/clang').read_bytes() == (stage / 'bin/clang').read_bytes()
    assert (prefix / 'bin/clang').stat().st_ino != (stage / 'bin/clang').stat().st_ino
