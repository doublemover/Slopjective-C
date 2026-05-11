"""Registration replay acceptance data contracts."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class RegistrationReplayArtifacts:
    provider_obj: Path
    consumer_obj: Path
    provider_import_surface: Path
    link_plan_path: Path
    provider_registration_manifest: JsonObject
    consumer_registration_manifest: JsonObject
    link_plan: JsonObject
    provider_compile_ms: int
    consumer_compile_ms: int


@dataclass(frozen=True)
class RegistrationReplayIdentities:
    provider: str
    consumer: str


@dataclass(frozen=True)
class RegistrationReplayProbeRun:
    payload: JsonObject
    probe_link_ms: int
    probe_run_ms: int


__all__ = [
    "JsonObject",
    "RegistrationReplayArtifacts",
    "RegistrationReplayIdentities",
    "RegistrationReplayProbeRun",
]
