from __future__ import annotations

import fnmatch
from collections.abc import Iterable
from pathlib import Path

from .roots import TEXT_SUFFIXES

EXTRA_TEXT_FILENAMES: frozenset[str] = frozenset({"CMakeLists.txt", "package.json"})


def normalize_path(path: Path, root: Path) -> str:
    return path.resolve().relative_to(root.resolve()).as_posix()


def is_excluded(repo_path: str, excludes: Iterable[str]) -> bool:
    normalized = repo_path.replace("\\", "/")
    return any(fnmatch.fnmatch(normalized, pattern) for pattern in excludes)


def is_text_candidate(path: Path) -> bool:
    return path.suffix.lower() in TEXT_SUFFIXES or path.name in EXTRA_TEXT_FILENAMES


def iter_scan_files(
    root: Path,
    scan_roots: Iterable[str],
    excludes: Iterable[str],
) -> Iterable[Path]:
    for raw_scan_root in scan_roots:
        scan_root = root / raw_scan_root
        if not scan_root.exists():
            continue
        candidates = [scan_root] if scan_root.is_file() else scan_root.rglob("*")
        for candidate in candidates:
            if not candidate.is_file():
                continue
            repo_path = normalize_path(candidate, root)
            if is_excluded(repo_path, excludes):
                continue
            if not is_text_candidate(candidate):
                continue
            yield candidate
