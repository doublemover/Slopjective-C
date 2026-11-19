import fcntl
import json
import sys
from pathlib import Path

from scripts.objc3c_toolchain import workspace_cleanup


def test_workspace_cleanup_requires_ownership_and_respects_live_lock(tmp_path, monkeypatch):
    root = tmp_path / 'scratch'
    root.mkdir()
    monkeypatch.setattr(workspace_cleanup, 'scratch_root', lambda repo: root)
    for name in ('run-expired', 'run-active', 'run-external'):
        directory = root / name
        directory.mkdir()
        (directory / '.lock').touch()
        (directory / workspace_cleanup.MARKER).write_text(json.dumps({
            'schema': workspace_cleanup.SCHEMA,
            'repo': str(tmp_path if name != 'run-external' else root),
            'owned_since': 0,
        }))
    with (root / 'run-active/.lock').open('rb') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        for apply in (False, True):
            result = workspace_cleanup.cleanup(tmp_path, apply=apply, older_than_hours=0)
            assert [Path(item['path']).name for item in result['disposable']] == ['run-expired']
            assert (root / 'run-expired').exists() != apply
            assert (root / 'run-active').is_dir() and (root / 'run-external').is_dir()


def test_work_directory_is_reclaimed_on_failed_diagnostic(tmp_path, monkeypatch):
    monkeypatch.setattr(workspace_cleanup, 'scratch_root', lambda repo: tmp_path)
    assert workspace_cleanup.run(tmp_path, [sys.executable, '-c',
        'import os; assert os.path.isdir(os.environ["TMPDIR"]); raise SystemExit(5)']) == 5
    assert not list(tmp_path.iterdir())
