"""Check the whole maintained patch series against only the touched upstream files."""

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


def check(repo: Path) -> tuple[int, int]:
    manifest, base = _load_manifest(repo / 'toolchains/llvm/manifest.json')
    patches = [path for _, path, _ in _series(manifest, repo, base, pristine=False)]
    inputs = upstream_paths(patches)
    commit = manifest['upstream']['commit']
    base_url = f'https://raw.githubusercontent.com/llvm/llvm-project/{commit}/'
    with tempfile.TemporaryDirectory(prefix='objc3-llvm-patch-check-') as temporary:
        root = Path(temporary)

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
        for patch in patches:
            apply = ['git', 'apply', '--whitespace=nowarn']
            subprocess.run([*apply, '--check', str(patch)], cwd=root, check=True)
            subprocess.run([*apply, str(patch)], cwd=root, check=True)
    return len(patches), len(inputs)


if __name__ == '__main__':
    patches, files = check(Path(__file__).resolve().parents[2])
    print(f'{patches} locked LLVM patches apply to the {files} pinned upstream files.')
