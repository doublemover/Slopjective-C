"""Open-blocker audit input validation and markdown scope resolution."""

from __future__ import annotations

import re
from datetime import datetime
from fnmatch import fnmatchcase
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

DEFAULT_EXCLUDE_PATHS: tuple[str, ...] = (
    ".git/**",
    ".github/**",
    ".pytest_cache/**",
    "node_modules/**",
    "reports/**",
    "tests/**",
    "tmp/**",
)

ISO_UTC_SECOND_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$")
WILDCARD_CHARS = set("*?[")


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def validate_generated_at_utc(raw_value: str) -> str:
    value = raw_value.strip()
    if value != raw_value:
        raise ValueError(
            "invalid --generated-at-utc: value must not include leading or trailing whitespace"
        )
    if not ISO_UTC_SECOND_RE.fullmatch(value):
        raise ValueError(
            "invalid --generated-at-utc: expected strict UTC timestamp like YYYY-MM-DDTHH:MM:SSZ"
        )
    try:
        datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise ValueError(
            "invalid --generated-at-utc: timestamp is not a valid UTC date-time"
        ) from exc
    return value


def validate_snapshot_source(raw_value: str) -> str:
    value = raw_value.strip()
    if not value:
        raise ValueError("invalid --source: value must be a non-empty canonical string")
    if value != raw_value:
        raise ValueError(
            "invalid --source: value must not include leading or trailing whitespace"
        )
    if normalize_space(value) != value:
        raise ValueError(
            "invalid --source: value must be canonical (no repeated internal whitespace)"
        )
    return value


def normalize_exclude_paths(
    user_values: Sequence[str],
    *,
    include_defaults: bool,
) -> tuple[str, ...]:
    source_values: list[str] = []
    if include_defaults:
        source_values.extend(DEFAULT_EXCLUDE_PATHS)
    source_values.extend(user_values)

    normalized: list[str] = []
    seen: set[str] = set()
    for raw in source_values:
        candidate = raw.strip().replace("\\", "/")
        if not candidate:
            raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
        if Path(candidate).is_absolute():
            raise ValueError("invalid --exclude-path: value must be repository-relative")
        while candidate.startswith("./"):
            candidate = candidate[2:]
        if not candidate:
            raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
        if candidate in seen:
            continue
        seen.add(candidate)
        normalized.append(candidate)

    return tuple(normalized)


def normalize_include_globs(raw_values: Sequence[str]) -> tuple[str, ...]:
    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        candidate = raw.strip().replace("\\", "/")
        if not candidate:
            raise ValueError("invalid --include-glob: value must be a non-empty glob pattern")
        if Path(candidate).is_absolute():
            raise ValueError("invalid --include-glob: value must be repository-relative")
        while candidate.startswith("./"):
            candidate = candidate[2:]
        if not candidate:
            raise ValueError("invalid --include-glob: value must be a non-empty glob pattern")
        if candidate in seen:
            continue
        seen.add(candidate)
        normalized.append(candidate)
    return tuple(normalized)


def split_static_prefix(glob_pattern: str) -> tuple[str, str]:
    parts = [part for part in glob_pattern.split("/") if part]
    prefix_parts: list[str] = []
    index = 0
    for part in parts:
        if any(character in part for character in WILDCARD_CHARS):
            break
        prefix_parts.append(part)
        index += 1
    prefix = "/".join(prefix_parts)
    remainder = "/".join(parts[index:]) if index < len(parts) else ""
    return prefix, remainder


def resolve_effective_audit_root(
    *,
    audit_root: Path,
    include_globs: Sequence[str],
) -> Path:
    if not include_globs:
        return audit_root

    static_prefixes: set[str] = set()
    allowed_remainders = {"", "**", "*.md", "**/*.md"}
    for include_glob in include_globs:
        static_prefix, remainder = split_static_prefix(include_glob)
        if not static_prefix:
            raise ValueError(
                "invalid --include-glob: each pattern must start with a static "
                "directory prefix (for example 'docs/reference/**/*.md')."
            )
        if remainder not in allowed_remainders:
            raise ValueError(
                "invalid --include-glob: only directory-scoped markdown globs are "
                f"supported (received {include_glob!r})."
            )
        static_prefixes.add(static_prefix)

    if len(static_prefixes) != 1:
        raise ValueError(
            "invalid --include-glob: patterns must share one static directory prefix "
            "in this runner contract."
        )

    prefix = next(iter(static_prefixes))
    return audit_root / Path(prefix)


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


def build_extractor_exclude_paths(
    *,
    exclude_paths: Sequence[str],
    markdown_paths: Sequence[str],
) -> tuple[str, ...]:
    markdown_parts = [tuple(path.split("/")) for path in markdown_paths]
    expanded: list[str] = []
    seen: set[str] = set()

    def add(pattern: str) -> None:
        if pattern in seen:
            return
        seen.add(pattern)
        expanded.append(pattern)

    def max_depth_for_prefix(prefix: str) -> int:
        if not prefix:
            return max((len(parts) for parts in markdown_parts), default=0)
        prefix_parts = tuple(part for part in prefix.split("/") if part)
        if not prefix_parts:
            return max((len(parts) for parts in markdown_parts), default=0)

        max_depth = 0
        for path_parts in markdown_parts:
            if len(path_parts) < len(prefix_parts):
                continue
            if path_parts[: len(prefix_parts)] != prefix_parts:
                continue
            depth = len(path_parts) - len(prefix_parts)
            if depth > max_depth:
                max_depth = depth
        return max_depth

    def expand_recursive_prefix(prefix: str) -> None:
        normalized_prefix = prefix.rstrip("/")
        depth_limit = max_depth_for_prefix(normalized_prefix)
        for depth in range(1, depth_limit + 1):
            stars = "/".join(["*"] * depth)
            if normalized_prefix:
                add(f"{normalized_prefix}/{stars}")
            else:
                add(stars)

    for pattern in exclude_paths:
        if pattern.endswith("/**/*"):
            expand_recursive_prefix(pattern[: -len("/**/*")])
            continue
        if pattern.endswith("/**"):
            expand_recursive_prefix(pattern[: -len("/**")])
            continue
        add(pattern)

    return tuple(expanded)


__all__ = [
    "DEFAULT_EXCLUDE_PATHS",
    "build_extractor_exclude_paths",
    "normalize_exclude_paths",
    "normalize_include_globs",
    "resolve_effective_audit_root",
    "resolve_markdown_scope",
    "validate_generated_at_utc",
    "validate_snapshot_source",
]
