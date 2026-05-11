"""Extractor exclude expansion for markdown audits."""

from __future__ import annotations

from typing import Sequence


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


__all__ = ("build_extractor_exclude_paths",)
