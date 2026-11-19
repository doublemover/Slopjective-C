"""Run diagnostics in owned scratch space and list or reclaim abandoned runs."""
from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


SCHEMA = 'objc3c.workspace-scratch.v1'
MARKER = '.objc3c-scratch.json'


def scratch_root(repo: Path) -> Path:
    identity = hashlib.sha256(os.fsencode(repo.resolve())).hexdigest()[:16]
    root = Path(tempfile.gettempdir()).resolve() / f'objc3c-{os.getuid()}-{identity}'
    root.mkdir(mode=0o700, exist_ok=True)
    if root.is_symlink() or root.stat().st_uid != os.getuid() or root.stat().st_mode & 0o077:
        raise ValueError('scratch root must be an owned private directory')
    return root


def allocated(path: Path) -> int:
    inodes = {}
    for member in (path, *path.rglob('*')):
        info = member.lstat()
        inodes[info.st_dev, info.st_ino] = info.st_blocks * 512
    return sum(inodes.values())


def run(repo: Path, command: list[str]) -> int:
    with tempfile.TemporaryDirectory(prefix='run-', dir=scratch_root(repo)) as work:
        work = Path(work)
        with (work / '.lock').open('xb') as lock:
            fcntl.flock(lock, fcntl.LOCK_EX)
            (work / MARKER).write_text(json.dumps({
                'schema': SCHEMA, 'repo': str(repo.resolve()), 'owned_since': time.time()}))
            return subprocess.run(command, env={**os.environ, 'TMPDIR': str(work)},
                                  pass_fds=(lock.fileno(),)).returncode


def cleanup(repo: Path, *, apply: bool, older_than_hours: int) -> dict:
    root = scratch_root(repo)
    results = []
    cutoff = time.time() - older_than_hours * 3600
    for work in sorted(root.glob('run-*')):
        if not work.is_dir() or work.is_symlink() or (work / MARKER).is_symlink():
            continue
        try:
            marker = json.loads((work / MARKER).read_bytes())
            if (marker['schema'] != SCHEMA or marker['repo'] != str(repo.resolve()) or
                    marker['owned_since'] > cutoff):
                continue
            descriptor = os.open(work / '.lock', os.O_RDONLY | os.O_NOFOLLOW)
            with os.fdopen(descriptor, 'rb') as lock:
                try:
                    fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
                except BlockingIOError:
                    continue
                results.append({'path': str(work), 'allocated_bytes': allocated(work)})
                if apply:
                    shutil.rmtree(work)
        except (ValueError, KeyError, FileNotFoundError):
            continue
    roots = [repo / 'out/toolchains', repo / 'out/diagnose-release-printer',
             repo / 'out/feature-sprint', repo / 'tmp', repo / 'artifacts']
    retained = {str(path): allocated(path) for path in roots if path.is_dir() and not path.is_symlink()}
    return {'applied': apply, 'scratch': str(root), 'disposable': results, 'build_output_inventory': retained,
            'workspace_available_bytes': shutil.disk_usage(repo).free,
            'scratch_available_bytes': shutil.disk_usage(root).free}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--older-than-hours', type=int, default=24)
    parser.add_argument('--run', nargs=argparse.REMAINDER, metavar='COMMAND')
    args = parser.parse_args()
    if args.run:
        return run(args.repo.resolve(), args.run)
    if args.older_than_hours < 0:
        parser.error('the age threshold must be nonnegative')
    print(json.dumps(cleanup(args.repo.resolve(), apply=args.apply, older_than_hours=args.older_than_hours),
                     indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
