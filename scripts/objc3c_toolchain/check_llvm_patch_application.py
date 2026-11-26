"""Check the maintained LLVM patches without building LLVM."""

import argparse
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path, PurePosixPath
import re
import shutil
import subprocess
import tarfile
import tempfile
import time
from urllib.error import HTTPError
from urllib.parse import quote
from urllib.request import urlopen

from .core import _load_manifest, _series


DIFF_FILES = re.compile(r'^--- (a/[^\n]+|/dev/null)\n^\+\+\+ (b/[^\n]+|/dev/null)$',
                        re.MULTILINE)
DIAGNOSTIC_PATTERN = r'(?:err|warn|ext|note|remark|fatal)_[A-Za-z0-9_]+'
DIAGNOSTIC_REFERENCE = re.compile(r'\bdiag::\s*(' + DIAGNOSTIC_PATTERN + r')\b')
DIAGNOSTIC_RECORD = re.compile(r'^\s*def\s+(' + DIAGNOSTIC_PATTERN + r')\s*:', re.MULTILINE)
DIAGNOSTIC_INCLUDE = re.compile(r'^\s*include\s+"(Diagnostic\w*\.td)"', re.MULTILINE)
DIAGNOSTIC_ROOT = Path('clang/include/clang/Basic')


def upstream_paths(patches: list[Path]) -> list[str]:
    first_appearance = {}
    for patch in patches:
        text = patch.read_text(encoding='utf-8')
        entries = list(DIFF_FILES.finditer(text))
        # Mode changes, renames, or quoted paths need their base files to be
        # resolved explicitly before this check can support those formats.
        if len(entries) != len(re.findall(r'^diff --git ', text, re.MULTILINE)):
            raise ValueError(f'{patch.name} contains unrecognized file headers')
        for match in entries:
            old, new = match.groups()
            deleted, created = new == '/dev/null', old == '/dev/null'
            name = new[2:] if created else old[2:]
            if (not name or name.startswith('/') or '..' in PurePosixPath(name).parts or
                    (not created and not deleted and name != new[2:])):
                raise ValueError(f'{patch.name} contains an unsupported patch path: {name}')
            first_appearance.setdefault(name, not created)
    return sorted(name for name, exists in first_appearance.items() if exists)


def pinned_inputs(repo: Path) -> tuple[str, list[Path]]:
    manifest, base = _load_manifest(repo / 'toolchains/llvm/manifest.json')
    patches = [path for _, path, _ in _series(manifest, repo, base, pristine=False)]
    return manifest['upstream']['commit'], patches


def fetch_upstream(root: Path, commit: str, inputs: list[str]) -> None:
    base_url = f'https://raw.githubusercontent.com/llvm/llvm-project/{commit}/'

    def fetch(name: str) -> None:
        url = base_url + quote(name)
        for attempt in range(3):
            try:
                with urlopen(url, timeout=30) as response:
                    contents = response.read()
                break
            except OSError as error:
                if attempt == 2 or (isinstance(error, HTTPError) and error.code == 429):
                    raise
                time.sleep(attempt + 1)
        file = root / name
        file.parent.mkdir(parents=True, exist_ok=True)
        file.write_bytes(contents)

    with ThreadPoolExecutor(max_workers=12) as pool:
        for _ in pool.map(fetch, inputs):
            pass


def fetch_upstream_archive(root: Path, commit: str, inputs: list[str]) -> None:
    """Stream the pinned tree into a directory containing only patch inputs and diagnostic tables."""
    url = f'https://codeload.github.com/llvm/llvm-project/tar.gz/{commit}'
    prefix = f'llvm-project-{commit}/'
    remaining = {*inputs, (DIAGNOSTIC_ROOT / 'Diagnostic.td').as_posix()}
    extracted = set()
    with urlopen(url, timeout=60) as response, tarfile.open(fileobj=response, mode='r|gz') as archive:
        for member in archive:
            if not member.name.startswith(prefix):
                continue
            name = member.name[len(prefix):]
            path = PurePosixPath(name)
            diagnostic = (path.parent == DIAGNOSTIC_ROOT and
                          path.name.startswith('Diagnostic') and path.suffix == '.td')
            if name not in remaining and name not in extracted and not diagnostic:
                continue
            if not member.isfile() or name != path.as_posix() or name in extracted:
                raise ValueError(f'unexpected LLVM archive entry: {name}')
            destination = root / path
            destination.parent.mkdir(parents=True, exist_ok=True)
            with archive.extractfile(member) as source, destination.open('wb') as output:
                shutil.copyfileobj(source, output)
            extracted.add(name)
            remaining.discard(name)
    if remaining:
        raise ValueError(f'LLVM archive {commit} is missing: {", ".join(sorted(remaining))}')


def check_diagnostic_references(patches: list[Path], tables: Path) -> int:
    definitions = {name for path in tables.glob('Diagnostic*.td')
                   for name in DIAGNOSTIC_RECORD.findall(path.read_text(encoding='utf-8'))}
    if not definitions:
        raise ValueError(f'no Clang diagnostic definitions found in {tables}')
    failures, references = [], set()
    for patch in patches:
        source = ''
        for number, line in enumerate(patch.read_text(encoding='utf-8').splitlines(), 1):
            if line.startswith('+++ '):
                source = line.removeprefix('+++ b/')
            elif line.startswith('+') and source.startswith(('clang/include/', 'clang/lib/', 'clang/tools/')):
                for name in DIAGNOSTIC_REFERENCE.findall(line[1:]):
                    references.add(name)
                    if name not in definitions:
                        failures.append(f'{patch.name}:{number}: {source} uses undefined clang::diag::{name}')
    if failures:
        raise ValueError('\n'.join(failures))
    return len(references)


def fetch_diagnostic_tables(root: Path, commit: str) -> None:
    tables = root / DIAGNOSTIC_ROOT
    pending = ['Diagnostic.td']
    try:
        while pending:
            fetch_upstream(root, commit, [(DIAGNOSTIC_ROOT / name).as_posix() for name in pending])
            pending = sorted({included for name in pending
                              for included in DIAGNOSTIC_INCLUDE.findall((tables / name).read_text(encoding='utf-8'))
                              if not (tables / included).is_file()})
    except HTTPError as error:
        if error.code != 429:
            raise
        fetch_upstream_archive(root, commit, [])


def check(repo: Path, *, diagnostics_only: bool = False) -> None:
    commit, patches = pinned_inputs(repo)
    darwin = repo / 'toolchains/llvm/experimental/darwin-registration.patch'
    apply = ['git', 'apply', '--whitespace=nowarn']
    with tempfile.TemporaryDirectory(prefix='objc3-llvm-patch-check-') as temporary:
        root = Path(temporary)
        if diagnostics_only:
            fetch_diagnostic_tables(root, commit)
            apply.append(f'--include={DIAGNOSTIC_ROOT}/Diagnostic*.td')
        else:
            inputs = upstream_paths([*patches, darwin])
            fetch_upstream_archive(root, commit, inputs)
        for patch in patches:
            subprocess.run([*apply, str(patch)], cwd=root, check=True)
        tables = root / DIAGNOSTIC_ROOT
        native = check_diagnostic_references(patches, tables)
        subprocess.run([*apply, str(darwin)], cwd=root, check=True)
        overlay = check_diagnostic_references([*patches, darwin], tables)
    if not diagnostics_only:
        print(f'{len(patches)} locked LLVM patches and the Darwin overlay apply to the {len(inputs)} pinned upstream files.')
    print(f'Clang diagnostic references resolved against the pinned tables ({native} Linux, {overlay} Darwin).')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--diagnostics-only', action='store_true',
                        help='check Clang diagnostic names against the patched TableGen records')
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[2]
    try:
        check(repo, diagnostics_only=args.diagnostics_only)
    except (OSError, ValueError, tarfile.TarError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'{error}\n')
