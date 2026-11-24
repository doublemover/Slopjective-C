"""A published archive outlives the mutable build candidate that produced it."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tarfile

import pytest

from scripts.objc3c_toolchain import linux_archive_reuse, linux_sdk, package
from scripts.objc3c_toolchain.linux_host import native_linux_host, native_target
from scripts.objc3c_toolchain.install_sdk import install
from scripts.objc3c_toolchain.sdk_example import write_json


IDENTITY = hashlib.sha256(b'current-inputs').hexdigest()


def cached_release(tmp_path, monkeypatch):
    repo = tmp_path / 'repo'
    foundation = tmp_path / 'foundation'
    (foundation / 'bin').mkdir(parents=True)
    (foundation / 'bin/objc3c').write_text('qualified compiler')
    (foundation / 'bin/objc3c').chmod(0o755)
    verifier = foundation / 'share/objc3c/verify-sdk.py'
    verifier.parent.mkdir(parents=True)
    shutil.copyfile(package.__file__, verifier)
    package.normalize_permissions(foundation)
    recipe = {'upstream_commit': 'test-upstream', 'patch_digest': 'test-patches',
              'build_config_digest': 'test-configuration'}
    data = {**recipe, 'schema': package.SCHEMA, 'source_build_id': 'test-release', 'target': native_target(),
            'linux_abi': {'glibc_minimum': '2.17', 'libstdcxx_symbol_minimum': 'GLIBCXX_3.4'},
            'files': package.inventory(foundation)}
    data['seal_sha256'] = package._digest(data)
    (foundation / package.MANIFEST).write_text(json.dumps(data))
    (foundation / package.MANIFEST).chmod(0o644)
    archive = repo / 'out/toolchains/sdk-cache/sdk.tar.zst'
    archive.parent.mkdir(parents=True)
    published = package._archive_sdk(foundation, archive, {
        'archive': str(archive), 'seal_sha256': data['seal_sha256'], 'source_build_id': 'test-release',
        'host': native_linux_host(), 'target': native_target()})
    directory = repo / 'out/toolchains/linux-sdk-builds' / IDENTITY
    directory.mkdir(parents=True)
    receipt = {**published, 'status': 'PASS', 'build_prefix': str(directory / 'prefix'), 'input_identity': IDENTITY}
    (directory / 'release.json').write_text(json.dumps(receipt))
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(foundation))
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda _: {**recipe, 'build_id': 'test-release'})
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *args: {'sha256': IDENTITY})
    return repo, foundation, receipt


def test_archived_release_is_reused_after_candidate_is_removed(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    assert not Path(receipt['build_prefix']).exists()
    result = linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert result['cache_hit']
    assert result['installed']['seal_sha256'] == receipt['seal_sha256']
    assert (repo / 'out/toolchains/linux-sdk/current').resolve() == Path(result['installed']['prefix'])
    package.verify_sdk(Path(result['installed']['prefix']), receipt['seal_sha256'])


def test_published_archive_owns_reuse_even_if_the_candidate_changes(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    candidate = Path(receipt['build_prefix'])
    shutil.copytree(foundation, candidate)
    (candidate / 'bin/objc3c').write_text('unpublished local edit')
    result = linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert result['cache_hit']
    assert (Path(result['installed']['prefix']) / 'bin/objc3c').read_text() == 'qualified compiler'


def test_corrupted_cached_archive_preserves_the_active_installation(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    root = repo / 'out/toolchains/linux-sdk'
    installed = install(Path(receipt['archive']), receipt['archive_sha256'], root)
    with Path(receipt['archive']).open('ab') as stream:
        stream.write(b'corrupted after publication')
    with pytest.raises(ValueError, match='SHA-256'):
        linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert (root / 'current').resolve() == Path(installed['prefix'])
    package.verify_sdk(Path(installed['prefix']), receipt['seal_sha256'])


def test_exact_replay_requires_existing_archive_admission(tmp_path, monkeypatch):
    repo, foundation, _ = cached_release(tmp_path, monkeypatch)
    with pytest.raises(ValueError, match='existing sealed prefix and archive admission'):
        linux_sdk.build_sdk(repo, foundation_sdk=foundation, reuse_input_identity=IDENTITY)
    assert not (repo / 'out/toolchains/linux-sdk/current').exists()


def test_same_run_replay_records_measured_readmission_without_build_or_extraction(tmp_path, monkeypatch):
    repo, _, receipt = cached_release(tmp_path, monkeypatch)
    root = repo / 'out/toolchains/linux-sdk'
    receipt['installed'] = install(Path(receipt['archive']), receipt['archive_sha256'], root)
    canonical = Path(receipt['build_prefix']).with_name('release.json')
    write_json(canonical, receipt)
    producer_receipt = repo / 'producer.json'
    write_json(producer_receipt, {**receipt, 'cache_hit': False})
    subprocess.run(['git', 'init', '-q'], cwd=repo, check=True)
    subprocess.run(['git', '-c', 'user.name=Archive Replay', '-c', 'user.email=replay@example.com',
                    'commit', '-qm', 'Producer revision', '--allow-empty'], cwd=repo, check=True)

    def reject_work(*args, **kwargs):
        pytest.fail('warm replay entered archive production, extraction or compilation')

    for name in ('build_foundation', 'copy_foundation', 'package_sdk', '_archive_sdk'):
        monkeypatch.setattr(linux_sdk, name, reject_work)
    monkeypatch.setattr(tarfile.TarFile, 'extractall', reject_work)
    output = repo / 'replay.json'
    assert linux_archive_reuse.main(['--repo', str(repo), '--producer-receipt', str(producer_receipt),
                                     '--output', str(output)]) == 0
    replay = json.loads(output.read_text())
    assert replay['status'] == 'PASS'
    assert replay['input_identity'] == IDENTITY
    assert replay['before'] == replay['after']
    assert replay['after']['archive'] == {'sha256': receipt['archive_sha256'], 'bytes': receipt['archive_bytes']}
    cost = replay['installation']['installed']['cost']
    assert cost['prefix_reused'] and cost['seal_sha256'] == receipt['seal_sha256']
    assert replay['stale_request']['status'] == 'REJECTED'
    assert replay['stale_request']['requested_input_identity'] != IDENTITY
    assert set(replay['phases_seconds']) == {'foundation_admission', 'qualification_identity',
                                            'receipt_and_archive_readmission'}
    assert 0 < sum(replay['phases_seconds'].values()) <= replay['replay_elapsed_seconds']
    linux_archive_reuse.verify_retained(output, producer_receipt, revision=replay['revision'], host=native_linux_host())

    sidecar = Path(receipt['archive']).with_suffix('.json')
    sidecar.write_text(json.dumps({**json.loads(sidecar.read_text()), 'seal_sha256': 'other'}))
    with pytest.raises(ValueError, match='package receipt differs'):
        linux_archive_reuse.qualify(repo, producer_receipt)
    assert (root / 'current').resolve() == Path(receipt['installed']['prefix'])


@pytest.fixture
def retained_replay(tmp_path):
    producer = {'status': 'PASS', 'cache_hit': False, 'input_identity': IDENTITY, 'host': 'linux-x86_64',
        'archive': '/producer/cache/sdk.tar.zst', 'archive_sha256': 'b'*64, 'archive_bytes': 123,
        'seal_sha256': 'c'*64, 'source_build_id': 'foundation-id', 'installed': {'prefix': '/producer/sdk'}}
    release = tmp_path / 'release.json'
    write_json(release, producer)
    canonical = tmp_path / 'canonical.json'
    write_json(canonical, {key: value for key, value in producer.items() if key != 'cache_hit'})
    keys = ('archive_sha256', 'archive_bytes', 'seal_sha256', 'source_build_id')
    sdk_identity = {key: producer[key] for key in keys}
    cost = {**sdk_identity, 'prefix_reused': True, 'schema': 'objc3c.sdk-install-cost.v1',
        'phases_seconds': dict.fromkeys(linux_archive_reuse.INSTALL_PHASES, .01), 'elapsed_seconds': .075}
    stale = hashlib.sha256(('stale release ' + IDENTITY).encode()).hexdigest()
    files = {'archive': {'bytes': producer['archive_bytes'], 'sha256': producer['archive_sha256']},
             'package_receipt': {'bytes': 123, 'sha256': 'd'*64},
             'producer_receipt': linux_archive_reuse.fingerprint(release),
             'release_receipt': linux_archive_reuse.fingerprint(canonical)}
    proof = {'schema': linux_archive_reuse.SCHEMA, 'status': 'PASS', 'revision': 'e'*40,
        'host': 'linux-x86_64', 'input_identity': IDENTITY, 'source_build_id': producer['source_build_id'],
        'archive_name': 'sdk.tar.zst', 'seal_sha256': producer['seal_sha256'], 'before': files, 'after': files,
        'installation': {**sdk_identity, 'status': 'PASS', 'installed': {'status': 'PASS', 'activated': True,
            'prefix': '/producer/sdk', 'seal_sha256': producer['seal_sha256'], 'cost': cost}},
        'phases_seconds': dict.fromkeys(linux_archive_reuse.PHASES, .1), 'replay_elapsed_seconds': .4,
        'stale_request': {'status': 'REJECTED', 'reason': 'qualification ' + stale,
            'requested_input_identity': stale, 'elapsed_seconds': .1}}
    return release, proof


@pytest.mark.parametrize('mutation', [None, 'missing', 'wrong-archive', 'wrong-seal', 'wrong-revision',
                                       'wrong-host', 'producer-bytes', 'missing-phase', 'rebuilt-prefix'])
def test_retained_archive_replay_proves_exact_package_inputs(tmp_path, retained_replay, mutation):
    release, proof = retained_replay
    path = tmp_path / 'archive-reuse.json'
    if mutation == 'wrong-archive':
        proof['before']['archive']['sha256'] = 'a'*64
    elif mutation in ('wrong-seal', 'wrong-revision', 'wrong-host'):
        proof[{'wrong-seal': 'seal_sha256', 'wrong-revision': 'revision', 'wrong-host': 'host'}[mutation]] = 'a'*64
    elif mutation == 'producer-bytes':
        release.write_bytes(release.read_bytes()+b'\n')
    elif mutation == 'missing-phase':
        proof['phases_seconds'].pop('foundation_admission')
    elif mutation == 'rebuilt-prefix':
        proof['installation']['installed']['cost']['prefix_reused'] = False
    if mutation != 'missing':
        write_json(path, proof)
    if mutation:
        with pytest.raises(ValueError, match='archive replay'):
            linux_archive_reuse.verify_retained(path, release, revision='e'*40, host='linux-x86_64')
    else:
        linux_archive_reuse.verify_retained(path, release, revision='e'*40, host='linux-x86_64')


@pytest.mark.parametrize(('host', 'complete'), [('linux-x86_64', True), ('linux-x86_64', False), ('linux-aarch64', False)])
def test_release_terminal_requires_and_charges_x86_archive_proof(tmp_path, monkeypatch, retained_replay, host, complete):
    from scripts.objc3c_assurance import evidence, release_matrix, sanitizer_matrix

    release, proof = retained_replay
    repo = tmp_path / 'repo'
    partial = repo / 'out/partials/product-producer'
    package = partial / 'package'
    package.mkdir(parents=True)
    shutil.copyfile(release, package / 'release.json')
    if complete:
        write_json(partial / 'archive-reuse.json', proof)
    plan = {'revision': {'tested': 'e'*40}, 'sha256': 'a'*64}
    write_json(partial / 'result.json', {'schema': evidence.SCHEMA, 'status': 'PASS',
        'revision': plan['revision'], 'plan_sha256': plan['sha256'], 'suite_partition': ['package'],
        'release_matrix_group': 'product-producer', 'product': {'archive_sha256': 'b'*64},
        'suites': [{'id': 'package', 'status': 'PASS', 'executions': ['passed'],
                    'artifacts': [evidence.artifact(partial, package / 'release.json')]}]})
    monkeypatch.setattr(release_matrix, 'GROUPS', {'product-producer': ('package',)})
    monkeypatch.setattr(release_matrix, 'native_linux_host', lambda: host)
    monkeypatch.setattr(release_matrix, 'validate', lambda _: None)
    monkeypatch.setattr(release_matrix, '_require_checkout', lambda *_: None)
    monkeypatch.setattr(release_matrix, 'selected_suites', lambda *_: [{'id': 'package'}])
    monkeypatch.setattr(release_matrix, 'suite_results', lambda *_: ['passed'])
    monkeypatch.setattr(release_matrix, 'snapshot', lambda **_: {'status': 'PASS'})
    monkeypatch.setattr(release_matrix, 'aggregate', lambda *_, **__: {'status': 'PASS'})
    monkeypatch.setattr(sanitizer_matrix, 'merge', lambda *_, **__: None)
    charged = []
    monkeypatch.setattr(release_matrix, 'validate_evidence_sizes', lambda _, size: charged.append(size))
    output = repo / 'out/aggregate'
    if host == 'linux-x86_64' and not complete:
        with pytest.raises(ValueError, match='missing its required archive replay receipt'):
            release_matrix.aggregate_core(repo, plan, partial.parent, output)
        return
    report = release_matrix.aggregate_core(repo, plan, partial.parent, output)
    assert ('archive_reuse' in report) is complete
    assert charged == [report['operational_costs']['bytes']+report.get('archive_reuse', {}).get('bytes', 0)]
    if complete:
        assert (output / 'archive-reuse.json').read_bytes() == (partial / 'archive-reuse.json').read_bytes()
