import errno
import hashlib
import io
import json
from compression import zstd
import tarfile
from pathlib import Path

import pytest

from scripts.objc3c_toolchain import stage_linux_release as owner


ROOT = Path(__file__).resolve().parents[2]
REPO = 'doublemover/Slopjective-C'
REVISION = 'a' * 40
RUNS = {'linux-x86_64': 123, 'linux-aarch64': 456}


@pytest.mark.parametrize('fault', [None, 'missing', 'failed', 'duplicate', 'image', 'archive', 'transcript'])
def test_clean_install_requires_every_scope_with_the_same_image_and_archive(tmp_path, monkeypatch, fault):
    install = owner.release_install
    monkeypatch.setenv('GITHUB_SHA', REVISION)
    monkeypatch.setenv('GITHUB_RUN_ID', '123')
    monkeypatch.setenv('GITHUB_RUN_ATTEMPT', '1')
    transcript = b'Sample execution transcript\n'
    for scope in install.SCOPES:
        if scope == 'smoke' and fault == 'missing':
            continue
        directory = tmp_path / scope
        directory.mkdir()
        row = {'schema': install.SCOPE_SCHEMA, 'status': 'PASS', 'host': 'linux-x86_64',
               'distribution': 'debian:13', 'revision': REVISION, 'run': 123, 'attempt': 1,
               'scope': scope, 'image': 'debian@sha256:' + 'a' * 64, 'image_id': 'sha256:' + 'b' * 64,
               'archive_sha256': 'c' * 64, 'seal_sha256': 'd' * 64, 'source_build_id': 'sdk-identity',
               'log_sha256': hashlib.sha256(transcript).hexdigest()}
        if scope == 'smoke':
            if fault == 'failed':
                row['status'] = 'FAIL'
            elif fault == 'image':
                row['image_id'] = 'sha256:' + 'e' * 64
            elif fault == 'archive':
                row['archive_sha256'] = 'e' * 64
        (directory / 'result.json').write_text(json.dumps(row))
        (directory / 'install.log').write_bytes(b'changed' if scope == 'smoke' and fault == 'transcript' else transcript)
    if fault == 'duplicate':
        (tmp_path / 'second').mkdir()
        for name in ('result.json', 'install.log'):
            (tmp_path / 'second' / name).write_bytes((tmp_path / 'smoke' / name).read_bytes())
    result = tmp_path / 'receipt.json'
    if fault:
        with pytest.raises(ValueError):
            install.aggregate(tmp_path, result, 'debian:13', host='linux-x86_64')
        assert not result.exists()
    else:
        install.aggregate(tmp_path, result, 'debian:13', host='linux-x86_64')
        record = json.loads(result.read_text())
        assert record['status'] == 'PASS'
        assert record['scopes'] == {
            scope: hashlib.sha256((tmp_path / scope / 'result.json').read_bytes()).hexdigest()
            for scope in install.SCOPES}


def api_run(run_id):
    return {'id': run_id, 'path': owner.QUALIFICATION, 'event': 'workflow_dispatch',
            'head_sha': REVISION, 'head_repository': {'full_name': REPO},
            'run_attempt': 1, 'status': 'completed', 'conclusion': 'success'}


def fixture_archive(directory, host, *, changed_installer=False, extension_abi=24):
    directory.mkdir(parents=True)
    manifest = {'schema': owner.SCHEMA, 'target': owner.HOSTS[host], 'source_build_id': host,
                'upstream_commit': 'b' * 40, 'patch_digest': 'c' * 64, 'linux_abi': {}}
    manifest['seal_sha256'] = owner._digest(manifest)
    payloads = {owner.MANIFEST: json.dumps(manifest).encode()}
    payloads[owner.FOUNDATION] = (
        f'#define CLANG_OBJC3_UPSTREAM_COMMIT "{manifest["upstream_commit"]}"\n'
        f'#define CLANG_OBJC3_EXTENSION_ABI_REVISION {extension_abi}\n').encode()
    payloads[owner.ARCHIVE_LICENSE] = b'fixture license'
    for name, path in owner.INSTALLERS.items():
        payloads[f'objc3-sdk/share/objc3c/{name}'] = (
            b'foreign installer' if changed_installer else (ROOT / path).read_bytes())
    archive = directory / 'fixture.tar.zst'
    with zstd.ZstdFile(archive, 'wb') as encoded:
        with tarfile.open(fileobj=encoded, mode='w|') as tar:
            for name, payload in payloads.items():
                member = tarfile.TarInfo(name)
                member.size = len(payload)
                tar.addfile(member, io.BytesIO(payload))
    archive.with_suffix('.json').write_text(json.dumps({
        **{key: manifest[key] for key in ('target', 'source_build_id', 'seal_sha256')},
        'host': host, 'archive_sha256': owner._file_hash(archive),
        'archive_bytes': archive.stat().st_size,
    }))
    return archive


@pytest.mark.parametrize('change', [
    {'conclusion': 'failure'}, {'head_sha': 'b' * 40}, {'path': 'another-workflow'},
    {'status': 'in_progress'}, {'head_repository': {'full_name': 'fork/Slopjective-C'}},
])
def test_rejects_wrong_qualification_run(change):
    with pytest.raises(ValueError, match='is not successful release qualification'):
        owner.validate_run({**api_run(123), **change}, repo=REPO, revision=REVISION, run_id=123)


def test_rejects_wrong_installer_and_archive_bytes(tmp_path):
    host = 'linux-x86_64'
    fixture_archive(tmp_path / 'foreign', host, changed_installer=True)
    with pytest.raises(ValueError, match='differs from the qualified revision'):
        owner.admit_archive(tmp_path / 'foreign', host=host, checkout=ROOT)
    archive = fixture_archive(tmp_path / 'broken', host)
    archive.write_bytes(b'not the downloaded archive')
    with pytest.raises(ValueError, match='differs from the downloaded bytes'):
        owner.admit_archive(archive.parent, host=host, checkout=ROOT)


@pytest.mark.parametrize('arm_abi,transport', [(24, 'same'), (24, 'cross-device'), (24, 'retry'), (21, 'same')])
def test_stages_two_native_archives_with_installer_and_checksums(tmp_path, monkeypatch, arm_abi, transport):
    downloads, output = tmp_path / 'downloads', tmp_path / 'draft'
    license_file = tmp_path / 'LICENSE'
    license_file.write_text('fixture license')
    monkeypatch.setattr(owner, 'distribution_license', lambda _: license_file)
    hosts = {str(run): host for host, run in RUNS.items()}
    monkeypatch.setattr(owner.subprocess, 'check_output',
                        lambda args: json.dumps(api_run(int(args[-1].rsplit('/', 1)[1]))))

    def download(command, *, check):
        assert check
        host = hosts[command[3]]
        directory = Path(command[-1])
        if command[-3].startswith('objc3c-linux-sdk-'):
            fixture_archive(directory, host, extension_abi=arm_abi if host == 'linux-aarch64' else 24)
            return
        distribution = next(value for value in owner.release_install.DISTRIBUTIONS
                            if value.replace(':', '-') == directory.name)
        receipt = json.loads((directory.parent / 'fixture.tar.json').read_bytes())
        directory.mkdir()
        (directory / 'result.json').write_text(json.dumps({
            'schema': owner.release_install.SCHEMA, 'status': 'PASS', 'host': host,
            'revision': REVISION, 'run': int(command[3]), 'attempt': 1,
            'distribution': distribution, 'image': 'fixture@sha256:' + '1' * 64,
            'image_id': 'sha256:' + '2' * 64,
            'scopes': {scope: '3' * 64 for scope in owner.release_install.SCOPES},
            **{key: receipt[key] for key in ('archive_sha256', 'seal_sha256', 'source_build_id')},
        }))

    monkeypatch.setattr(owner.subprocess, 'run', download)
    if transport != 'same':
        def fail_link(path, original):
            if transport == 'retry' and 'aarch64' in path.name:
                raise OSError(errno.ENOSPC, 'injected out of space')
            raise OSError(errno.EXDEV, 'cross-device link')
        if transport == 'retry':
            with monkeypatch.context() as patch:
                patch.setattr(Path, 'hardlink_to', fail_link)
                with pytest.raises(OSError, match='injected out of space'):
                    owner.stage(ROOT, repo=REPO, revision=REVISION, tag='v0.1.0-preview.1',
                                runs=RUNS, downloads=downloads, output=output)
            assert not output.exists()
            assert len(list(downloads.rglob('*.tar.zst'))) == 2
            monkeypatch.setattr(owner.subprocess, 'run', lambda *args, **kwargs: pytest.fail('downloads lost'))
        else:
            monkeypatch.setattr(Path, 'hardlink_to', fail_link)
    if arm_abi != 24:
        with pytest.raises(ValueError, match='different compiler source stacks'):
            owner.stage(ROOT, repo=REPO, revision=REVISION, tag='v0.1.0-preview.1',
                        runs=RUNS, downloads=downloads, output=output)
        assert not output.exists()
        return
    result = owner.stage(ROOT, repo=REPO, revision=REVISION, tag='v0.1.0-preview.1',
                         runs=RUNS, downloads=downloads, output=output)
    assert result['profiles_claimed'] == []
    assert {host: row['qualification_run'] for host, row in result['archives'].items()} == RUNS
    assert {row['extension_abi'] for row in result['archives'].values()} == {24}
    assert all(len(row['clean_installs']) == 2 for row in result['archives'].values())
    checksums = dict(line.split('  ')[::-1] for line in (output / 'SHA256SUMS').read_text().splitlines())
    assert set(checksums) == {p.name for p in output.iterdir()} - {'SHA256SUMS'}
    for name, digest in checksums.items():
        assert hashlib.sha256((output / name).read_bytes()).hexdigest() == digest


@pytest.mark.parametrize('changed', [False, True])
def test_resumes_only_matching_remote_assets(tmp_path, monkeypatch, changed):
    bundle = tmp_path / 'bundle'
    bundle.mkdir()
    manifest, installer = bundle / 'linux-release.json', bundle / 'install-linux.sh'
    manifest.write_text('{}\n')
    installer.write_text('fixture installer')
    tag = 'v0.1.0-preview.1'
    draft = {'id': 4, 'tag_name': tag, 'draft': True, 'prerelease': True, 'target_commitish': REVISION,
             'body': f'<!-- objc3c-release-manifest-sha256: {owner._file_hash(manifest)} -->'}
    assets = [{'name': manifest.name, 'size': manifest.stat().st_size,
               'digest': f'sha256:{owner._file_hash(manifest)}' if not changed else 'sha256:old'}]

    def api(command):
        route = command[2].split(REPO + '/', 1)[1]
        return json.dumps({'git/matching-refs/tags/' + tag: [],
                           'releases?per_page=100': [[draft]], 'releases/4/assets': assets}[route])

    def upload(command, *, check):
        assert check and command == ['gh', 'release', 'upload', tag, '--repo', REPO, str(installer)]
        assets.append({'name': installer.name, 'size': installer.stat().st_size,
                       'digest': f'sha256:{owner._file_hash(installer)}'})

    monkeypatch.setattr(owner.subprocess, 'check_output', api)
    monkeypatch.setattr(owner.subprocess, 'run', upload)
    if changed:
        with pytest.raises(ValueError, match='mismatched asset'):
            owner.publish_draft(bundle, repo=REPO, tag=tag, revision=REVISION, notes=tmp_path / 'notes')
        assert len(assets) == 1
    else:
        owner.publish_draft(bundle, repo=REPO, tag=tag, revision=REVISION, notes=tmp_path / 'notes')
        assert len(assets) == 2
