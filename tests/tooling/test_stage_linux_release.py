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


def api_run(run_id):
    return {'id': run_id, 'path': owner.QUALIFICATION, 'event': 'workflow_dispatch',
            'head_sha': REVISION, 'head_repository': {'full_name': REPO},
            'status': 'completed', 'conclusion': 'success'}


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


@pytest.mark.parametrize('arm_abi', [24, 21])
def test_stages_two_native_archives_with_installer_and_checksums(tmp_path, monkeypatch, arm_abi):
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
        fixture_archive(Path(command[-1]), host, extension_abi=arm_abi if host == 'linux-aarch64' else 24)

    monkeypatch.setattr(owner.subprocess, 'run', download)
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
    checksums = dict(line.split('  ')[::-1] for line in (output / 'SHA256SUMS').read_text().splitlines())
    assert set(checksums) == {p.name for p in output.iterdir()} - {'SHA256SUMS'}
    for name, digest in checksums.items():
        assert hashlib.sha256((output / name).read_bytes()).hexdigest() == digest
