"""Materialize only the exact owned changes on pinned Linux runtime source."""
from __future__ import annotations

import hashlib
from pathlib import Path
import subprocess


def locked_patches(repo: Path, profile: dict) -> list[Path]:
    rows = profile.get('source_patches')
    if not isinstance(rows, list) or not rows:
        raise ValueError('Linux runtime requires a pinned source patch inventory')
    paths = []
    for row in rows:
        if not isinstance(row, dict) or set(row) != {'file', 'sha256'}:
            raise ValueError('invalid runtime source patch record')
        if not isinstance(row['file'], str) or not isinstance(row['sha256'], str):
            raise ValueError('runtime patch paths and checksums must be strings')
        path = (repo / row['file']).resolve(strict=True)
        if not path.is_relative_to(repo.resolve()) or path in paths:
            raise ValueError('runtime source patch is external or duplicated')
        if hashlib.sha256(path.read_bytes()).hexdigest() != row['sha256']:
            raise ValueError(f'runtime source patch checksum changed: {path}')
        paths.append(path)
    return paths


def verify_source(repo: Path, source: Path, profile: dict, *, allow_pristine=False) -> bool:
    patches = locked_patches(repo, profile)
    def git(*args):
        return subprocess.check_output(['git', '-C', str(source), *args])
    revision = git('rev-parse', 'HEAD').decode().strip()
    if revision != profile['runtime_commit']:
        raise ValueError('runtime source revision differs from its profile pin')
    if git('ls-files', '--others', '--exclude-standard', '-z'):
        raise ValueError('runtime source contains unowned files; preserving it')
    actual = git('diff', '--binary', '--abbrev=9', '--no-ext-diff', 'HEAD')
    if actual == b''.join(path.read_bytes() for path in patches):
        return True
    if allow_pristine and not actual:
        return False
    raise ValueError('runtime source differs from the locked patches; preserving it')


def prepare_source(repo: Path, source: Path, profile: dict) -> None:
    if verify_source(repo, source, profile, allow_pristine=True):
        return
    patches = locked_patches(repo, profile)
    subprocess.run(['git', '-C', str(source), 'apply', '--whitespace=error',
                    *map(str, patches)], check=True)
    verify_source(repo, source, profile)
