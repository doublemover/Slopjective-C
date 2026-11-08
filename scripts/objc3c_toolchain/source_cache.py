"""Reuse pinned pristine Git objects; materialize independent source checkouts."""
from __future__ import annotations

from contextlib import contextmanager
import fcntl
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


class SourceCacheError(ValueError):
    pass


def git(path: Path, *arguments: str) -> str:
    result = subprocess.run(['git', '-C', str(path), *arguments], text=True,
                            capture_output=True, check=False)
    if result.returncode:
        raise SourceCacheError(result.stderr.strip() or result.stdout.strip())
    return result.stdout.strip()


@contextmanager
def lock(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('a+b') as stream:
        fcntl.flock(stream, fcntl.LOCK_EX)
        yield


def verify(cache: Path, *, tag: str, commit: str, tag_object: str | None = None) -> None:
    if cache.is_symlink() or git(cache, 'rev-parse', '--is-bare-repository') != 'true':
        raise SourceCacheError('source cache must be an owned bare Git repository')
    if tag_object is not None and git(cache, 'rev-parse', f'refs/tags/{tag}') != tag_object:
        raise SourceCacheError('cached upstream tag object differs from its pin')
    if git(cache, 'rev-parse', f'refs/tags/{tag}^{{commit}}') != commit:
        raise SourceCacheError('cached upstream tag resolves to a different commit')
    # Include blob integrity, not merely the presence of a commit name.
    git(cache, 'fsck', '--full', '--no-dangling')


def ensure(cache: Path, *, url: str, tag: str, commit: str,
           tag_object: str | None = None, fetch: bool = False) -> dict:
    cache = cache.absolute()
    expected = {'schema': 'objc3c.pristine-source-cache.v1', 'url': url,
                'tag': tag, 'commit': commit, 'tag_object': tag_object}
    with lock(cache.with_name(cache.name + '.lock')):
        if cache.exists():
            try:
                verify(cache, tag=tag, commit=commit, tag_object=tag_object)
                return {**expected, 'cache_hit': True, 'path': str(cache)}
            except SourceCacheError:
                marker = cache / 'objc3c-source-cache.json'
                # Replace only this installer's disposable, obsolete cache.
                if not fetch or cache.is_symlink() or not marker.is_file():
                    raise
                recorded = json.loads(marker.read_text())
                if recorded.get('schema') != expected['schema'] or recorded.get('url') != url:
                    raise SourceCacheError('unrecognized source cache; preserving its contents')
                shutil.rmtree(cache)
        if not fetch:
            raise SourceCacheError(f'missing pinned source cache {cache}; use --fetch to acquire it')
        cache.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='.source-cache-', dir=cache.parent) as temporary:
            candidate = Path(temporary) / 'source.git'
            subprocess.run(['git', 'init', '--bare', str(candidate)], check=True,
                           stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
            git(candidate, 'remote', 'add', 'origin', url)
            git(candidate, 'fetch', '--depth', '1', 'origin', f'refs/tags/{tag}:refs/tags/{tag}')
            verify(candidate, tag=tag, commit=commit, tag_object=tag_object)
            (candidate / 'objc3c-source-cache.json').write_text(json.dumps(expected, indent=2) + '\n')
            os.replace(candidate, cache)
        return {**expected, 'cache_hit': False, 'path': str(cache)}


def checkout(cache: Path, destination: Path, *, url: str, tag: str, commit: str,
             tag_object: str | None = None, fetch: bool = False) -> dict:
    if destination.exists():
        raise SourceCacheError('source cache checkout requires a fresh destination')
    receipt = ensure(cache, url=url, tag=tag, commit=commit, tag_object=tag_object, fetch=fetch)
    # No alternates or borrowed hardlinks: cache eviction cannot break a build.
    subprocess.run(['git', 'clone', '--no-hardlinks', '--no-checkout', str(cache), str(destination)],
                   check=True, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    git(destination, 'checkout', '--detach', commit)
    git(destination, 'remote', 'set-url', 'origin', url)
    return receipt
