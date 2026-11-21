"""Materialize the maintained LLDB parser header for source-free frontend tests."""
from __future__ import annotations
import argparse
from pathlib import Path
import re
from .core import _load_manifest, _series

HEADER = 'lldb/source/Plugins/ExpressionParser/Clang/ObjC3MethodNames.h'


def materialize(repo: Path, output: Path) -> Path:
    manifest, base = _load_manifest(repo / 'toolchains/llvm/manifest.json')
    matches = []
    for _, patch, _ in _series(manifest, repo, base, pristine=False):
        for section in patch.read_text().split('diff --git ')[1:]:
            lines = section.splitlines()
            if lines[0] != f'a/{HEADER} b/{HEADER}':
                continue
            hunk = next((i for i, line in enumerate(lines) if line.startswith('@@ ')), None)
            if hunk is None or '--- /dev/null' not in lines[:hunk]:
                raise ValueError('debugger test header must be a complete maintained addition')
            count = re.fullmatch(r'@@ -0,0 \+1,(\d+) @@', lines[hunk])
            body = lines[hunk + 1:]
            if not count or len(body) != int(count[1]) or any(not line.startswith('+') for line in body):
                raise ValueError('debugger test header patch is incomplete')
            matches.append('\n'.join(line[1:] for line in body) + '\n')
    if len(matches) != 1:
        raise ValueError('debugger test header must have exactly one locked owner')
    target = output / HEADER.removeprefix('lldb/source/')
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(matches[0])
    return target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    materialize(args.repo.resolve(strict=True), args.output.resolve())


if __name__ == '__main__':
    main()
