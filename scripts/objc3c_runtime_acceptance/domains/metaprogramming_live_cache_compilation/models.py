"""Data models for live metaprogramming host-cache compilation cases."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class LiveMetaprogrammingCacheProvider:
    fixture: Path
    source: str
    module_name: str
    unique_suffix: str
    cache_root: Path
    cache_root_override: str
    cache_root_args: tuple[str, ...]


@dataclass(frozen=True)
class LiveMetaprogrammingCacheCompile:
    compile_dir: Path
    host_cache_artifact_path: Path
    runtime_import_path: Path
    host_cache_artifact: JsonObject
    runtime_import_surface: JsonObject
    host_cache_import_surface: JsonObject


@dataclass(frozen=True)
class LiveMetaprogrammingCacheConsumerLink:
    compile_dir: Path
    link_plan_path: Path
    link_plan: JsonObject


@dataclass(frozen=True)
class LiveMetaprogrammingCacheProbeRun:
    executable_path: Path
    payload: JsonObject


@dataclass(frozen=True)
class ProviderSource:
    source: str
    module_name: str
    unique_suffix: str


__all__ = [
    "JsonObject",
    "LiveMetaprogrammingCacheCompile",
    "LiveMetaprogrammingCacheConsumerLink",
    "LiveMetaprogrammingCacheProbeRun",
    "LiveMetaprogrammingCacheProvider",
    "ProviderSource",
]
