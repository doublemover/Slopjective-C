"""Compile artifact models for Object Model metaclass samples."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class MetaclassGraphCompileArtifacts:
    obj_path: Path
    ll_path: Path
    manifest_path: Path
    ll_text: str
    manifest: dict[str, Any]


@dataclass(frozen=True)
class CanonicalSampleCompileArtifacts:
    obj_path: Path
    ll_path: Path
    manifest_path: Path
    registration_manifest_path: Path
    ll_text: str
    manifest: dict[str, Any]
    registration_manifest: dict[str, Any]


__all__ = [
    "CanonicalSampleCompileArtifacts",
    "MetaclassGraphCompileArtifacts",
]
