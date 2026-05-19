"""Compiler artifact acceptance data shapes."""

from __future__ import annotations

import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class CompileBackendParityEvidence:
    fixture: Path
    direct_backend: str
    wrapper_backend: str
    direct_provenance: dict[str, Any]
    direct_truthfulness: dict[str, Any]
    compared_truthfulness_fields: list[str]


@dataclass(frozen=True)
class RegistryEventSnapshot:
    reuse_events: int
    miss_events: int


@dataclass(frozen=True)
class ArtifactRegistryCacheEvidence:
    changed_args_key_a: str
    changed_args_key_b: str
    changed_args_payload_a: dict[str, Any]
    changed_args_payload_b: dict[str, Any]
    changed_import_surface_key_a: str
    changed_import_surface_key_b: str
    changed_import_surface_payload_a: dict[str, Any]
    changed_import_surface_payload_b: dict[str, Any]


@dataclass(frozen=True)
class ArtifactRegistryCaseEvidence:
    fixture: Path
    first_result: subprocess.CompletedProcess[str]
    second_result: subprocess.CompletedProcess[str]
    third_result: subprocess.CompletedProcess[str]
    reuse_events_before: int
    miss_events_before: int
    reuse_events_after_distinct_args: int
    reuse_events_after_same_args: int
    miss_events_after: int
    cache: ArtifactRegistryCacheEvidence
