"""Check the maintained LLVM patches without building LLVM."""

import argparse
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path, PurePosixPath
import re
import subprocess
import tempfile
import time
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
            except OSError:
                if attempt == 2:
                    raise
                time.sleep(attempt + 1)
        file = root / name
        file.parent.mkdir(parents=True, exist_ok=True)
        file.write_bytes(contents)

    with ThreadPoolExecutor(max_workers=12) as pool:
        for _ in pool.map(fetch, inputs):
            pass


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


def check_diagnostics(repo: Path) -> tuple[int, int]:
    commit, patches = pinned_inputs(repo)
    darwin = repo / 'toolchains/llvm/experimental/darwin-registration.patch'
    with tempfile.TemporaryDirectory(prefix='objc3-llvm-diagnostics-') as temporary:
        root = Path(temporary)
        tables = root / DIAGNOSTIC_ROOT
        pending = ['Diagnostic.td']
        while pending:
            fetch_upstream(root, commit, [str(DIAGNOSTIC_ROOT / name) for name in pending])
            pending = sorted({included for name in pending
                              for included in DIAGNOSTIC_INCLUDE.findall((tables / name).read_text(encoding='utf-8'))
                              if not (tables / included).is_file()})
        apply = ['git', 'apply', '--whitespace=nowarn', f'--include={DIAGNOSTIC_ROOT}/Diagnostic*.td']
        for patch in patches:
            subprocess.run([*apply, str(patch)], cwd=root, check=True)
        native = check_diagnostic_references(patches, tables)
        subprocess.run([*apply, str(darwin)], cwd=root, check=True)
        return native, check_diagnostic_references([*patches, darwin], tables)


def check(repo: Path) -> tuple[int, int]:
    commit, patches = pinned_inputs(repo)
    inputs = upstream_paths(patches)
    with tempfile.TemporaryDirectory(prefix='objc3-llvm-patch-check-') as temporary:
        root = Path(temporary)
        fetch_upstream(root, commit, inputs)
        for patch in patches:
            apply = ['git', 'apply', '--whitespace=nowarn']
            subprocess.run([*apply, '--check', str(patch)], cwd=root, check=True)
            subprocess.run([*apply, str(patch)], cwd=root, check=True)
    return len(patches), len(inputs)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--diagnostics-only', action='store_true',
                        help='check Clang diagnostic names against the patched TableGen records')
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[2]
    if args.diagnostics_only:
        try:
            native, darwin = check_diagnostics(repo)
        except (OSError, ValueError, subprocess.CalledProcessError) as error:
            parser.exit(1, f'{error}\n')
        print(f'Clang diagnostic references resolved against the pinned tables ({native} Linux, {darwin} Darwin).')
    else:
        patches, files = check(repo)
        print(f'{patches} locked LLVM patches apply to the {files} pinned upstream files.')
