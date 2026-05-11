"""Data models for compiler artifact acceptance case execution."""

from __future__ import annotations

import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class CompileBackendRun:
    result: subprocess.CompletedProcess[str]
    selected_backend: str


@dataclass(frozen=True)
class CompileBackendParityLayout:
    fixture: Path
    case_dir: Path
    direct_dir: Path
    wrapper_dir: Path
    parity_cache_root: Path
    parity_args: tuple[str, ...]


@dataclass(frozen=True)
class CompileBackendParityArtifacts:
    direct_provenance: JsonObject
    wrapper_provenance: JsonObject
    direct_truthfulness: JsonObject
    wrapper_truthfulness: JsonObject
    compared_truthfulness_fields: tuple[str, ...]


@dataclass(frozen=True)
class ArtifactRegistryIsolationLayout:
    fixture: Path
    case_dir: Path
    producer_dir: Path
    distinct_args_dir: Path
    consumer_dir: Path
    args_a: tuple[str, ...]
    args_b: tuple[str, ...]
    backend: str


__all__ = [
    "ArtifactRegistryIsolationLayout",
    "CompileBackendParityArtifacts",
    "CompileBackendParityLayout",
    "CompileBackendRun",
    "JsonObject",
]
