"""Markdown scope discovery for open-blocker audits."""

from __future__ import annotations

from fnmatch import fnmatchcase
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path


def path_matches_glob(relative_path: str, glob: str) -> bool:
    return fnmatchcase(relative_path, glob)


def resolve_markdown_scope(
    *,
    audit_root: Path,
    exclude_paths: Sequence[str],
) -> tuple[list[str], list[str]]:
    if not audit_root.exists():
        raise ValueError(f"audit root path does not exist: {display_path(audit_root)}")
    if not audit_root.is_dir():
        raise ValueError(f"audit root path is not a directory: {display_path(audit_root)}")

    resolved_root = audit_root.resolve()
    included: list[str] = []
    excluded: list[str] = []

    markdown_paths = sorted(
        (
            path.resolve()
            for path in resolved_root.rglob("*.md")
            if path.is_file()
        ),
        key=lambda path: (
            path.relative_to(resolved_root).as_posix().casefold(),
            path.relative_to(resolved_root).as_posix(),
        ),
    )

    for resolved_path in markdown_paths:
        relative_path = resolved_path.relative_to(resolved_root).as_posix()
        if any(path_matches_glob(relative_path, pattern) for pattern in exclude_paths):
            excluded.append(relative_path)
            continue
        included.append(relative_path)

    return included, excluded


__all__ = (
    "path_matches_glob",
    "resolve_markdown_scope",
)
