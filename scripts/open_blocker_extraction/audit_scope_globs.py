"""Glob normalization and audit-root resolution."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from .audit_scope_constants import DEFAULT_EXCLUDE_PATHS, WILDCARD_CHARS


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


__all__ = (
    "normalize_exclude_paths",
    "normalize_include_globs",
    "resolve_effective_audit_root",
    "split_static_prefix",
)
