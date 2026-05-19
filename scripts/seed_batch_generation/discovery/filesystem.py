"""Filesystem scanning for seed matrix source candidates."""

from __future__ import annotations

from collections.abc import Iterable
from pathlib import Path

from .candidates import MatrixSourceCandidate, build_source_candidate
from .constants import MATRIX_SOURCE_SUFFIX


def iter_matrix_source_candidates(
    root: Path,
    *,
    file_names: Iterable[str] | None = None,
) -> list[MatrixSourceCandidate]:
    resolved_root = root.resolve(strict=False)
    expected_names = frozenset(file_names or ())
    if not resolved_root.exists():
        return []
    if resolved_root.is_file():
        candidate_root = resolved_root.parent
        raw_paths = [resolved_root]
    else:
        candidate_root = resolved_root
        raw_paths = [
            path
            for path in resolved_root.rglob(f"*{MATRIX_SOURCE_SUFFIX}")
            if path.is_file()
        ]

    candidates = [
        build_source_candidate(path, candidate_root)
        for path in raw_paths
        if _matches_expected_name(path, expected_names)
    ]
    return sorted(candidates, key=lambda candidate: candidate.sort_key)


def _matches_expected_name(path: Path, expected_names: frozenset[str]) -> bool:
    if path.suffix.lower() != MATRIX_SOURCE_SUFFIX:
        return False
    if not expected_names:
        return True
    return path.name in expected_names
