"""Path normalization and markdown file discovery for OPEN blocker extraction."""

from __future__ import annotations

from fnmatch import fnmatchcase
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path


def normalize_exclude_pattern(raw_pattern: str) -> str:
    if raw_pattern != raw_pattern.strip():
        raise ValueError(
            "invalid --exclude-path: value must not include leading or trailing whitespace"
        )
    normalized = raw_pattern.replace("\\", "/")
    if not normalized:
        raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
    if Path(normalized).is_absolute():
        raise ValueError("invalid --exclude-path: value must be repository-relative")
    while normalized.startswith("./"):
        normalized = normalized[2:]
    if not normalized:
        raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
    return normalized


def normalize_exclude_patterns(raw_patterns: Sequence[str]) -> tuple[str, ...]:
    normalized = {
        normalize_exclude_pattern(raw_pattern)
        for raw_pattern in raw_patterns
    }
    return tuple(sorted(normalized, key=lambda value: (value.casefold(), value)))


def iter_markdown_files(
    root: Path,
    *,
    exclude_patterns: Sequence[str],
) -> list[Path]:
    root_resolved = root.resolve()

    def is_excluded(path: Path) -> bool:
        if not exclude_patterns:
            return False

        candidate_paths = {display_path(path)}
        resolved = path.resolve()
        try:
            root_relative = resolved.relative_to(root_resolved).as_posix()
            candidate_paths.add(root_relative)
        except ValueError:
            pass
        return any(
            fnmatchcase(candidate_path, pattern)
            for candidate_path in candidate_paths
            for pattern in exclude_patterns
        )

    return sorted(
        (
            path
            for path in root.rglob("*.md")
            if path.is_file()
            and not is_excluded(path)
        ),
        key=lambda path: (display_path(path).casefold(), display_path(path)),
    )
