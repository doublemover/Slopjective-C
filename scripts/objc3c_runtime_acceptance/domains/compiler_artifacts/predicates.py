"""Compiler artifact acceptance predicates and readers."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.runtime_artifact_registry import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
)

from .catalog import ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX
from .data import RegistryEventSnapshot


def read_json_object(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def registry_event_snapshot() -> RegistryEventSnapshot:
    return RegistryEventSnapshot(
        reuse_events=len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events),
        miss_events=len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events),
    )


def selected_backend_is_direct(selected_backend: str) -> bool:
    return selected_backend == DIRECT_COMPILE_BACKEND


def registry_reuse_count_unchanged(before: int, after: int) -> bool:
    return after == before


def registry_reused_exactly_once(before: int, after: int) -> bool:
    return after == before + 1


def reuse_event_matches_paths(
    reuse_event: dict[str, Any],
    *,
    producer_dir: Path,
    consumer_dir: Path,
) -> bool:
    return (
        reuse_event.get("producer_dir") == repo_display_path(producer_dir)
        and reuse_event.get("consumer_dir") == repo_display_path(consumer_dir)
    )


def artifact_registry_cache_key(
    fixture: Path,
    *,
    extra_args: list[str],
) -> tuple[str, dict[str, Any]]:
    return ACCEPTANCE_ARTIFACT_REGISTRY.cache_key(
        fixture,
        extra_args=extra_args,
        backend=DIRECT_COMPILE_BACKEND,
        emit_prefix=ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX,
    )


def artifact_cache_keys_are_distinct(
    changed_args_key_a: str,
    changed_args_key_b: str,
    changed_import_surface_key_a: str,
    changed_import_surface_key_b: str,
) -> bool:
    return (
        changed_args_key_a != changed_args_key_b
        and changed_import_surface_key_a != changed_import_surface_key_b
    )
