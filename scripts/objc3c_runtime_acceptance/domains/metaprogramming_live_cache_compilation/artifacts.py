"""Artifact loading helpers for live metaprogramming host-cache cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .config import (
    CONSUMER_LINK_PLAN_FILE,
    HOST_CACHE_ARTIFACT_FILE,
    HOST_CACHE_IMPORT_SURFACE_KEY,
    RUNTIME_IMPORT_SURFACE_FILE,
)
from .models import (
    JsonObject,
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
)


def load_provider_compile_artifacts(
    compile_dir: Path,
) -> LiveMetaprogrammingCacheCompile:
    host_cache_artifact_path = compile_dir / HOST_CACHE_ARTIFACT_FILE
    runtime_import_path = compile_dir / RUNTIME_IMPORT_SURFACE_FILE
    host_cache_artifact = load_json_object(host_cache_artifact_path)
    runtime_import_surface = load_json_object(runtime_import_path)
    host_cache_import_surface = _optional_json_object(
        runtime_import_surface.get(HOST_CACHE_IMPORT_SURFACE_KEY),
        HOST_CACHE_IMPORT_SURFACE_KEY,
    )
    return LiveMetaprogrammingCacheCompile(
        compile_dir=compile_dir,
        host_cache_artifact_path=host_cache_artifact_path,
        runtime_import_path=runtime_import_path,
        host_cache_artifact=host_cache_artifact,
        runtime_import_surface=runtime_import_surface,
        host_cache_import_surface=host_cache_import_surface,
    )


def load_consumer_link_plan(compile_dir: Path) -> LiveMetaprogrammingCacheConsumerLink:
    link_plan_path = compile_dir / CONSUMER_LINK_PLAN_FILE
    return LiveMetaprogrammingCacheConsumerLink(
        compile_dir=compile_dir,
        link_plan_path=link_plan_path,
        link_plan=load_json_object(link_plan_path),
    )


def load_json_object(path: Path) -> JsonObject:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if isinstance(payload, dict):
        return payload
    raise TypeError(f"expected JSON object in {path}")


def _optional_json_object(payload: Any, label: str) -> JsonObject:
    if payload is None:
        return {}
    if isinstance(payload, dict):
        return payload
    raise TypeError(f"expected JSON object for {label}")


__all__ = [
    "load_consumer_link_plan",
    "load_json_object",
    "load_provider_compile_artifacts",
]
