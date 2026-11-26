import errno
import hashlib
import io
import json
import os
import shutil
from compression import zstd
import subprocess
import tarfile
from pathlib import Path

import pytest

from scripts.objc3c_toolchain import package, preview_support, stage_linux_release as owner


ROOT = Path(__file__).resolve().parents[2]
REPO = 'doublemover/Slopjective-C'
REVISION = 'a' * 40
RUNS = {'linux-x86_64': 123, 'linux-aarch64': 456}


@pytest.mark.parametrize('defect', [None, 'target', 'host', 'revision', 'attempt', 'seal', 'duplicate', 'llvm'])
def test_preview_matrix_projects_only_exact_native_clean_install_receipts(defect):
    release = json.loads((ROOT / preview_support.MANIFEST).read_bytes())
    archive = release['archives']['linux-aarch64']
    first = archive['clean_installs'][0]
    if defect == 'target':
        archive['target'] = 'arm64-apple-macos15.0'
    elif defect == 'host':
        first['host'] = 'linux-x86_64'
    elif defect == 'revision':
        first['revision'] = REVISION
    elif defect == 'attempt':
        first['attempt'] += 1
    elif defect == 'seal':
        first['seal_sha256'] = '0' * 64
    elif defect == 'duplicate':
        archive['clean_installs'][1] = first
    elif defect == 'llvm':
        archive['upstream_commit'] = REVISION
    if defect:
        with pytest.raises(ValueError, match='archive coordinates|receipt differs|run or distribution|LLVM base version'):
            preview_support.render(release)
    else:
        assert preview_support.render(release) == (ROOT / preview_support.TABLE).read_text()


@pytest.mark.skipif(not shutil.which('zstd'), reason='release rehearsal needs zstd')
@pytest.mark.parametrize(('defect', 'error'), [
    (None, None),
    ('archive', 'archive SHA-256 differs from the published manifest'),
    ('seal_sha256', 'SDK admission differs from the declared seal'),
    ('source_build_id', 'SDK admission differs from the declared seal'),
    ('changed companion', 'installer companion differs from the qualified SDK'),
    ('missing companion', 'installer companion differs from the qualified SDK'),
    ('checksum omission', 'checksum index omitted or changed a required asset'),
    ('checksum duplicate', 'checksum index has an invalid or duplicate entry'),
])
def test_draft_rehearsal_admits_exact_sealed_published_installers(tmp_path, monkeypatch, defect, error):
    host = owner.release_install.host_identity()['host']
    release = json.loads((ROOT / preview_support.MANIFEST).read_bytes())
    archive = release['archives'][host]
    prefix = tmp_path / 'sdk'
    members = prefix / 'share/objc3c'
    members.mkdir(parents=True)
    shutil.copyfile(package.__file__, members / 'verify-sdk.py')
    for name in owner.release_install.INSTALLER_COMPANIONS:
        (members / name).write_text(f'Qualified {name}\n')
        shutil.copyfile(members / name, tmp_path / name)
    package.normalize_permissions(prefix)
    sdk = {'schema': package.SCHEMA, 'source_build_id': archive['source_build_id'],
           'target': archive['target'], 'files': package.inventory(prefix)}
    sdk['seal_sha256'] = package._digest(sdk)
    manifest = prefix / package.MANIFEST
    manifest.write_text(json.dumps(sdk))
    manifest.chmod(0o644)
    archive_path = tmp_path / archive['file']
    packed = package._archive_sdk(prefix, archive_path, {'seal_sha256': sdk['seal_sha256']})
    archive.update(bytes=packed['archive_bytes'], sha256=packed['archive_sha256'], seal_sha256=sdk['seal_sha256'])
    if defect in ('seal_sha256', 'source_build_id'):
        archive[defect] = '0' * 64
    for row in archive['clean_installs']:
        row.update(archive_sha256=archive['sha256'], seal_sha256=archive['seal_sha256'],
                   source_build_id=archive['source_build_id'])
    (tmp_path / 'linux-release.json').write_text(json.dumps(release))
    if defect == 'archive':
        contents = archive_path.read_bytes()
        archive_path.write_bytes(contents[:-1] + bytes([contents[-1] ^ 1]))
    elif defect == 'changed companion':
        (tmp_path / 'install-sdk.py').write_text('Different standalone installer\n')
    elif defect == 'missing companion':
        (tmp_path / 'install-sdk.py').unlink()
    files = [archive_path, tmp_path / 'linux-release.json', tmp_path / 'install-linux.sh']
    if defect not in ('missing companion', 'checksum omission'):
        files += [tmp_path / 'install-sdk.py']
    if defect == 'checksum duplicate':
        files += [tmp_path / 'install-linux.sh']
    # A rewritten index can agree with a substituted asset.
    (tmp_path / 'SHA256SUMS').write_text(''.join(f'{package._file_hash(file)}  {file.name}\n' for file in files))
    receipts = {item['image']: item['image_id'] for item in archive['clean_installs']}
    resolved, commands = [], []

    def image(reference):
        resolved.append(reference)
        return reference, receipts[reference]

    monkeypatch.setattr(owner.release_install, '_container', image)
    monkeypatch.setattr(owner.release_install.subprocess, 'run', lambda command, *, check: commands.append(command))
    if error:
        with pytest.raises(ValueError, match=error):
            owner.release_install.rehearse(tmp_path, host)
        assert not resolved and not commands
    else:
        owner.release_install.rehearse(tmp_path, host)
        assert resolved == list(receipts)
        assert len(commands) == len(receipts)
        for command in commands:
            assert command[-4:] == [archive[key] for key in ('file', 'sha256', 'seal_sha256', 'source_build_id')]
            assert 'activated SDK differs from the declared release seal or source build' in command[-6]


def test_draft_rehearsal_rejects_a_non_native_host_before_reading_release(tmp_path):
    native = owner.release_install.host_identity()['host']
    other, = set(owner.HOSTS) - {native}
    with pytest.raises(ValueError, match='differs from the native host'):
        owner.release_install.rehearse(tmp_path, other)


@pytest.mark.parametrize('case', ['valid', 'already provisioned', 'bad digest', 'missing archive',
                                 'missing companion', 'bad option', 'help'])
def test_release_bootstrap_admits_the_public_pair_and_archive_before_provisioning(tmp_path, case):
    assets = tmp_path / 'downloaded release'
    assets.mkdir()
    launcher = assets / 'install-linux.sh'
    shutil.copyfile(ROOT / 'scripts/install_objc3c_linux.sh', launcher)
    companion = assets / 'install-sdk.py'
    if case != 'missing companion':
        companion.write_text('Public companion fixture\n')
    nested = assets / 'objc3c_toolchain'
    nested.mkdir()
    (nested / 'install_sdk.py').write_text('Nested checkout fixture\n')
    archive = assets / 'objc3-sdk-v0.1.0-preview.1-linux-x86_64.tar.zst'
    if case != 'missing archive':
        archive.write_bytes(b'release archive input')
    trace = tmp_path / 'executions'
    commands = tmp_path / 'bin'
    commands.mkdir()
    for name, body in {
        'id': 'printf "0\\n"\n',
        'dpkg-query': ('shift 2; for package do printf "installed\\n"; done\n'
                       if case == 'already provisioned' else 'exit 1\n'),
        'apt-get': 'printf "apt %s\\n" "$1" >> "$TRACE"\n',
        'python3': 'printf "%s\\n" "$@" >> "$TRACE"\n',
    }.items():
        command = commands / name
        command.write_text('#!/bin/sh\n' + body)
        command.chmod(0o700)
    digest = '0' * 64 if case == 'bad digest' else hashlib.sha256(b'release archive input').hexdigest().upper()
    arguments = ['--archive=' + str(archive), '--sha256', digest, '--root', str(tmp_path / 'installed')]
    if case == 'bad option':
        arguments.append('--unknown')
    elif case == 'help':
        arguments = ['--help']
    result = subprocess.run(['sh', str(launcher), '--provision', *arguments], capture_output=True, text=True,
                            env={**os.environ, 'PATH': f'{commands}:{os.environ["PATH"]}', 'TRACE': str(trace)},
                            timeout=15)
    if case in ('valid', 'already provisioned'):
        assert result.returncode == 0, result.stderr
        setup = [] if case == 'already provisioned' else ['apt update', 'apt install']
        assert trace.read_text().splitlines() == [*setup, str(companion), *arguments]
    elif case == 'help':
        assert result.returncode == 0 and 'Usage:' in result.stdout
        assert not trace.exists()
    else:
        assert result.returncode == 2 and result.stderr.startswith('objc3c install:')
        assert not trace.exists()


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


def api_job(run_id, *, name=owner.TERMINAL_GATE, conclusion='success', job_id=41):
    return {'run_id': run_id, 'run_attempt': 1, 'head_sha': REVISION, 'id': job_id,
            'name': name, 'status': 'completed', 'conclusion': conclusion,
            'started_at': '2026-11-21T10:00:00Z', 'completed_at': '2026-11-21T10:00:42Z'}


@pytest.mark.parametrize('fault', [None, 'wrong-revision', 'wrong-attempt', 'missing-gate',
                                  'duplicate-id', 'failed-job', 'missing-page'])
def test_captures_passed_qualification_jobs(tmp_path, monkeypatch, fault):
    run = api_run(123)
    first = api_job(123, name='optional job', conclusion='skipped')
    second = api_job(123, job_id=42)
    if fault == 'wrong-revision':
        second['head_sha'] = 'b' * 40
    if fault == 'wrong-attempt':
        second['run_attempt'] = 2
    if fault == 'missing-gate':
        second['name'] = 'some other successful job'
    if fault == 'duplicate-id':
        second['id'] = first['id']
    if fault == 'failed-job':
        second['conclusion'] = 'failure'
    total = 3 if fault == 'missing-page' else 2

    def api(command):
        assert command == ['gh', 'api', f'repos/{REPO}/actions/runs/123/attempts/1/jobs?per_page=100',
                           '--paginate', '--jq', '{total_count,jobs}']
        return '\n'.join(json.dumps({'total_count': total, 'jobs': [row]}) for row in (first, second))

    monkeypatch.setattr(owner.subprocess, 'check_output', api)
    if fault:
        with pytest.raises(ValueError, match='attempt 1'):
            owner.qualification_jobs(REPO, run)
    else:
        skipped, passed = owner.qualification_jobs(REPO, run)
        assert skipped == {'id': 41, 'name': 'optional job', 'conclusion': 'skipped'}
        assert passed['name'] == owner.TERMINAL_GATE and passed['seconds'] == 42


def fixture_archive(directory, host, *, changed_installer=False, extension_abi=24):
    directory.mkdir(parents=True)
    manifest = {'schema': owner.SCHEMA, 'target': owner.HOSTS[host], 'source_build_id': host,
                'upstream_commit': 'b' * 40, 'patch_digest': 'c' * 64, 'build_config_digest': 'd' * 64, 'linux_abi': {}}
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


def test_checkout_compares_the_full_configuration_identity(tmp_path, monkeypatch):
    host = 'linux-x86_64'
    archive = fixture_archive(tmp_path / 'fixture', host)
    license_file = tmp_path / 'LICENSE'
    license_file.write_text('fixture license')
    monkeypatch.setattr(owner, 'distribution_license', lambda _: license_file)
    _, record = owner.admit_archive(archive.parent, host=host, checkout=ROOT)
    identity = {'upstream_commit': record['upstream_commit'], 'patch_digest': record['patch_digest'],
                'build_id': record['source_build_id'], 'build_config_digest': record['build_config_digest']}
    monkeypatch.setattr(owner, 'planned_development_identity', lambda _: identity)
    monkeypatch.setattr(owner.subprocess, 'check_output', lambda *args, **kwargs: REVISION)
    monkeypatch.setattr(owner.subprocess, 'run', lambda *args, **kwargs: subprocess.CompletedProcess([], 0))
    owner.admit_checkout(ROOT, revision=REVISION, archives={host: record})
    record['build_config_digest'] = record['build_config_digest'][:8] + 'a'*56
    with pytest.raises(ValueError, match='differs? from the pinned compiler source stack'):
        owner.admit_checkout(ROOT, revision=REVISION, archives={host: record})


@pytest.mark.parametrize('arm_abi,transport', [(24, 'same'), (24, 'cross-device'), (24, 'retry'), (21, 'same')])
def test_stages_two_native_archives_with_installer_and_checksums(tmp_path, monkeypatch, arm_abi, transport):
    downloads, output = tmp_path / 'downloads', tmp_path / 'draft'
    license_file = tmp_path / 'LICENSE'
    license_file.write_text('fixture license')
    monkeypatch.setattr(owner, 'distribution_license', lambda _: license_file)
    monkeypatch.setattr(owner, 'admit_checkout', lambda *args, **kwargs: None)

    def source_inventory(checkout, *, revision, tag, archives):
        assert checkout == ROOT and revision == REVISION and archives.keys() == RUNS.keys()
        return [{'component': 'Slopjective-C', 'revision': revision, 'file': f'slopjective-c-{tag}-source.tar.gz'}]

    monkeypatch.setattr(owner, 'source_inventory', source_inventory)

    def stage_sources(checkout, bundle, *, revision, tag, archives):
        assert checkout == ROOT and revision == REVISION and archives.keys() == RUNS.keys()
        source, = source_inventory(checkout, revision=revision, tag=tag, archives=archives)
        file = bundle / source['file']
        file.write_bytes(b'fixture source archive')
        return [{**source, 'sha256': owner._file_hash(file), 'bytes': file.stat().st_size}]

    monkeypatch.setattr(owner, 'stage_sources', stage_sources)
    hosts = {str(run): host for host, run in RUNS.items()}

    def api(command):
        run, *tail = command[2].split('/actions/runs/', 1)[1].split('/')
        if tail:
            assert tail == ['attempts', '1', 'jobs?per_page=100']
            return json.dumps({'total_count': 1, 'jobs': [api_job(int(run))]})
        return json.dumps(api_run(int(run)))

    monkeypatch.setattr(owner.subprocess, 'check_output', api)

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
    assert all(row['qualification_jobs'][0]['name'] == owner.TERMINAL_GATE
               for row in result['archives'].values())
    checksums = dict(line.split('  ')[::-1] for line in (output / 'SHA256SUMS').read_text().splitlines())
    assert set(checksums) == {p.name for p in output.iterdir()} - {'SHA256SUMS'}
    assert checksums[result['sources'][0]['file']] == result['sources'][0]['sha256']
    for name, digest in checksums.items():
        assert hashlib.sha256((output / name).read_bytes()).hexdigest() == digest
    monkeypatch.setattr(owner, 'stage_sources', lambda *args, **kwargs: pytest.fail('valid staged sources were recreated'))
    monkeypatch.setattr(owner.subprocess, 'run', lambda *args, **kwargs: pytest.fail('valid staged SDK was downloaded again'))
    assert owner.stage(ROOT, repo=REPO, revision=REVISION, tag='v0.1.0-preview.1',
                       runs=RUNS, downloads=downloads, output=output) == result
    (output / 'LICENSE').write_bytes(b'changed license')
    with pytest.raises(ValueError, match='differs from its checksum'):
        owner.stage(ROOT, repo=REPO, revision=REVISION, tag='v0.1.0-preview.1',
                    runs=RUNS, downloads=downloads, output=output)


@pytest.mark.parametrize('fault', [None, 'missing-license', 'foreign-root', 'truncated'])
def test_pinned_dependency_source_archive_requires_root_and_license(tmp_path, monkeypatch, fault):
    buffer = io.BytesIO()
    root = 'libobjc2-' + 'd' * 40
    with tarfile.open(fileobj=buffer, mode='w:gz') as archive:
        name = 'README' if fault == 'missing-license' else 'COPYING'
        member = tarfile.TarInfo(f'{"other" if fault == "foreign-root" else root}/{name}')
        payload = b'license contents'
        member.size = len(payload)
        archive.addfile(member, io.BytesIO(payload))
    data = buffer.getvalue()
    monkeypatch.setattr(owner, 'urlopen', lambda url, timeout: io.BytesIO(data[:-4] if fault == 'truncated' else data))
    target = tmp_path / 'source.tar.gz'
    if fault:
        with pytest.raises(EOFError if fault == 'truncated' else ValueError):
            owner.download_source('fixture', target, root=root, license_path='COPYING')
    else:
        owner.download_source('fixture', target, root=root, license_path='COPYING')
        assert target.read_bytes() == data


@pytest.fixture
def draft_fixture(tmp_path, monkeypatch):
    bundle = tmp_path / 'bundle'
    bundle.mkdir()
    (bundle / 'linux-release.json').write_text('{}\n')
    (bundle / 'install-linux.sh').write_text('fixture installer')
    (bundle / 'SHA256SUMS').write_text(''.join(
        f'{owner._file_hash(path)}  {path.name}\n' for path in sorted(bundle.iterdir())))
    notes = tmp_path / 'notes.md'
    notes.write_text('Fixture draft notes\n')
    tag = 'v0.1.0-preview.1'
    draft = {'id': 4, 'tag_name': tag, 'draft': True, 'prerelease': True, 'target_commitish': REVISION,
             'body': f'<!-- objc3c-release-manifest-sha256: {owner._file_hash(bundle / "linux-release.json")} -->'}
    assets = {}
    mutations = []
    slept = []
    state = {'exists': True, 'hidden': 0, 'asset_lag': 0, 'finish_starter': False,
             'failed_asset': None, 'release_edited': False}

    def asset(name, *, state='uploaded'):
        path = bundle / name
        return {'id': len(assets)+20, 'name': name, 'size': path.stat().st_size if state == 'uploaded' else 0,
                'digest': f'sha256:{owner._file_hash(path)}' if state == 'uploaded' else None, 'state': state}

    def api(command):
        route = command[2].split(REPO + '/', 1)[1]
        if route == 'releases?per_page=100':
            assert command[-3:] == ['--paginate', '--jq', '.[]']
            if state['hidden']:
                state['hidden'] -= 1
                return ''
            return json.dumps(draft) if state['exists'] else ''
        if route == 'releases/4/assets?per_page=100':
            assert command[-3:] == ['--paginate', '--jq', '.[]']
            if state['asset_lag'] and any(kind == 'upload' for kind, _ in mutations):
                state['asset_lag'] -= 1
                return ''
            return '\n'.join(map(json.dumps, assets.values()))
        if route == 'releases/4':
            return json.dumps(draft)
        if route.startswith('releases/assets/'):
            row = next(row for row in assets.values() if row['id'] == int(route.split('/')[-1]))
            if state['finish_starter']:
                row.update({**asset(row['name']), 'id': row['id']})
            return json.dumps(row)
        assert route == 'git/matching-refs/tags/' + tag
        return '[]'

    def mutate(command, *, check):
        assert check
        if command[:3] == ['gh', 'release', 'create']:
            assert not state['exists']
            mutations.append(('create', tag))
            state.update(exists=True, hidden=2)
            draft['body'] = Path(command[-1]).read_text()
        elif command[:3] == ['gh', 'release', 'upload']:
            name = Path(command[-1]).name
            assert command == ['gh', 'release', 'upload', tag, '--repo', REPO, str(bundle / name)]
            assert name not in assets
            mutations.append(('upload', name))
            if state['failed_asset'] == name:
                assets[name] = asset(name, state='starter')
                raise subprocess.CalledProcessError(1, command)
            assets[name] = asset(name)
            if state['release_edited']:
                draft['draft'] = False
        else:
            assert command[:4] == ['gh', 'api', '--method', 'DELETE'] and command[-1] == '--silent'
            asset_id = int(command[-2].split('/')[-1])
            name, = [key for key, row in assets.items() if row['id'] == asset_id and row['state'] == 'starter']
            mutations.append(('delete-starter', name))
            del assets[name]

    def publish():
        owner.publish_draft(bundle, repo=REPO, tag=tag, revision=REVISION, notes=notes)

    monkeypatch.setattr(owner.subprocess, 'check_output', api)
    monkeypatch.setattr(owner.subprocess, 'run', mutate)
    monkeypatch.setattr(owner.time, 'sleep', slept.append)
    return bundle, draft, assets, asset, state, mutations, slept, publish


def test_resumes_only_matching_remote_assets(draft_fixture):
    _, _, assets, asset, _, actions, _, publish = draft_fixture
    manifest = 'linux-release.json'
    assets[manifest] = asset(manifest)
    publish()
    assert actions == [('upload', 'SHA256SUMS'), ('upload', 'install-linux.sh')]
    publish()
    assert actions == [('upload', 'SHA256SUMS'), ('upload', 'install-linux.sh')]


@pytest.mark.parametrize('fault', ['changed', 'unknown', 'duplicate', 'starter-bytes', 'bundle-marker'])
def test_rejects_unowned_remote_assets_before_any_mutation(draft_fixture, fault):
    _, draft, assets, asset, _, actions, _, publish = draft_fixture
    manifest = 'linux-release.json'
    assets[manifest] = asset(manifest)
    if fault == 'changed':
        assets[manifest]['digest'] = 'sha256:changed'
    elif fault == 'unknown':
        assets['another'] = {**assets[manifest], 'name': 'unexpected.zip'}
    elif fault == 'duplicate':
        assets['another'] = {**assets[manifest], 'id': 999}
    elif fault == 'starter-bytes':
        assets[manifest].update(state='starter', digest=None)
    else:
        draft['body'] += '\n<!-- objc3c-release-bundle-sha256: changed -->'
    with pytest.raises(ValueError, match='asset|exact resumable preview draft'):
        publish()
    assert actions == []


def test_interrupted_upload_repairs_only_its_empty_placeholder(draft_fixture):
    _, _, assets, _, state, actions, _, publish = draft_fixture
    state['failed_asset'] = 'install-linux.sh'
    with pytest.raises(subprocess.CalledProcessError):
        publish()
    assert actions == [('upload', 'SHA256SUMS'), ('upload', 'install-linux.sh')]
    assert assets['install-linux.sh']['state'] == 'starter'
    state['failed_asset'] = None
    publish()
    assert actions[2:] == [('delete-starter', 'install-linux.sh'), ('upload', 'install-linux.sh'),
                          ('upload', 'linux-release.json')]
    assert all(asset['state'] == 'uploaded' for asset in assets.values())


def test_waits_for_the_new_draft_and_records_the_whole_bundle_identity(draft_fixture):
    bundle, draft, _, _, state, actions, slept, publish = draft_fixture
    state.update(exists=False, asset_lag=2)
    publish()
    assert actions[0][0] == 'create'
    assert slept == [0, 1, 2, 0, 1, 2]
    assert f'<!-- objc3c-release-bundle-sha256: {owner._file_hash(bundle / "SHA256SUMS")} -->' in draft['body']


def test_final_verification_rejects_remote_draft_edited_during_upload(draft_fixture):
    _, _, assets, asset, state, actions, _, publish = draft_fixture
    for name in ('linux-release.json', 'SHA256SUMS'):
        assets[name] = asset(name)
    state['release_edited'] = True
    with pytest.raises(ValueError, match='exact resumable preview draft'):
        publish()
    assert actions == [('upload', 'install-linux.sh')]


def test_interrupted_marker_which_finished_is_preserved(draft_fixture):
    _, _, assets, asset, state, actions, _, publish = draft_fixture
    assets['install-linux.sh'] = asset('install-linux.sh', state='starter')
    state['finish_starter'] = True
    publish()
    assert assets['install-linux.sh']['state'] == 'uploaded'
    assert actions == [('upload', 'SHA256SUMS'), ('upload', 'linux-release.json')]


def test_expected_bundle_rejects_changed_installer_before_remote_operations(draft_fixture):
    bundle, _, _, _, _, actions, _, publish = draft_fixture
    (bundle / 'install-linux.sh').write_text('changed installer')
    with pytest.raises(ValueError, match='staged asset differs from its checksum'):
        publish()
    assert actions == []
