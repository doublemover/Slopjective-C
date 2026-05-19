"""Validation helpers for the performance-governance source surface."""

from __future__ import annotations

import sys
from collections.abc import Callable, Mapping, Sequence
from pathlib import Path


FailureHandler = Callable[[str], int]


def fail(message: str) -> int:
    print(f"performance-governance-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def require_exact_path(
    source_surface: Mapping[str, object],
    field_name: str,
    *,
    expected_required_paths: Mapping[str, str],
    fail_handler: FailureHandler = fail,
) -> str | None:
    expected_path = expected_required_paths[field_name]
    if source_surface.get(field_name) != expected_path:
        fail_handler(f"{field_name} drifted from required path {expected_path}")
        return None
    return expected_path


def require_path(
    relative_path: str,
    *,
    kind: str,
    root: Path,
    fail_handler: FailureHandler = fail,
) -> bool:
    path = root / relative_path
    if not path.exists():
        fail_handler(f"missing {kind}: {relative_path}")
        return False
    return True


def require_exact_list(
    source_surface: Mapping[str, object],
    field_name: str,
    expected_items: Sequence[str],
    *,
    fail_handler: FailureHandler = fail,
) -> Sequence[str] | None:
    if source_surface.get(field_name) != list(expected_items):
        fail_handler(f"{field_name} drifted from required entries")
        return None
    return expected_items


def require_exact_owner_split(
    source_surface: Mapping[str, object],
    *,
    expected_owner_split: dict[str, list[str]],
    fail_handler: FailureHandler = fail,
) -> dict[str, list[str]] | None:
    if source_surface.get("owner_split") != expected_owner_split:
        fail_handler("owner_split drifted from required performance ownership boundaries")
        return None
    return expected_owner_split
