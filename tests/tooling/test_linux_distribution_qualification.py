"""Fresh release failures cannot retain passing evidence or owned containers."""
import json
from types import SimpleNamespace

import pytest

from scripts import qualify_linux_distributions as qualification


def release_inputs(tmp_path):
    archive = tmp_path / 'sdk.tar.zst'
    archive.write_bytes(b'archive')
    release = tmp_path / 'release.json'
    release.write_text(json.dumps({'status': 'PASS', 'archive': str(archive),
        'archive_sha256': qualification.digest(archive), 'seal_sha256': 'a' * 64}))
    output = tmp_path / 'evidence'
    output.mkdir()
    (output / 'qualification.json').write_text('{"status":"PASS"}')
    return archive, release, output


@pytest.mark.parametrize('failure', ['consumer', 'archive-mutated', 'cleanup'])
def test_failed_fresh_release_replaces_pass_and_cleans_owned_container(
        tmp_path, monkeypatch, failure):
    archive, release, output = release_inputs(tmp_path)
    commands = []

    def run(argv, **kwargs):
        commands.append(argv)
        code = 0
        if 'exec' in argv:
            if failure == 'consumer':
                code = 1
                kwargs['stdout'].write('consumer failed\n')
            else:
                kwargs['stdout'].write('Unprivileged Linux native feature consumer: PASS\n'
                                       'Fresh Linux native feature qualification: PASS\n')
            if failure == 'archive-mutated':
                archive.write_bytes(b'changed archive')
        elif 'container' in argv and failure == 'cleanup':
            code = 1
        return SimpleNamespace(returncode=code)

    monkeypatch.setattr(qualification.subprocess, 'run', run)
    with pytest.raises(ValueError):
        qualification.qualify(release, output, distributions=['debian13'])
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL'
    record = report['distributions'][0]
    assert record['container_removed'] is (failure != 'cleanup')
    assert commands[-1][-4:] == ['container', 'rm', '--force', record['container']]
    assert record['container'].startswith('objc3-features-debian13-')
    assert len(commands) == 3


def test_archive_mismatch_fails_before_container_creation(tmp_path, monkeypatch):
    archive, release, output = release_inputs(tmp_path)
    archive.write_bytes(b'corrupt archive')

    def unexpected(*args, **kwargs):
        pytest.fail('archive admission must precede Docker')

    monkeypatch.setattr(qualification.subprocess, 'run', unexpected)
    with pytest.raises(ValueError, match='archive digest differs'):
        qualification.qualify(release, output, distributions=['debian13'])
    report = json.loads((output / 'qualification.json').read_text())
    assert report['status'] == 'FAIL' and not report['distributions']
