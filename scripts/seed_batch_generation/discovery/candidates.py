"""Candidate models for seed matrix discovery."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class MatrixSourceCandidate:
    """A deterministic candidate source path for a seed matrix manifest."""

    path: Path
    display_path: str
    sort_key: tuple[str, ...]


def build_source_candidate(path: Path, root: Path) -> MatrixSourceCandidate:
    resolved_path = path.resolve(strict=False)
    resolved_root = root.resolve(strict=False)
    try:
        display_path = resolved_path.relative_to(resolved_root).as_posix()
    except ValueError:
        display_path = resolved_path.as_posix()
    return MatrixSourceCandidate(
        path=resolved_path,
        display_path=display_path,
        sort_key=tuple(part.lower() for part in display_path.split("/")),
    )
