"""Block/ARC capture legality data model."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


DiagnosticBatch = dict[str, Any]
ManifestSurface = dict[str, Any]


@dataclass(frozen=True)
class SurfaceProfile:
    escape_to_heap_sites: int
    requires_byref_cells_sites: int
    copy_helper_required_sites: int
    dispose_helper_required_sites: int


@dataclass(frozen=True)
class CaptureFixtureSpec:
    key: str
    fixture: Path
    output_dir_name: str
    expected_profile: SurfaceProfile
    escape_message: str
    copy_dispose_message: str


@dataclass(frozen=True)
class CaptureFixtureFacts:
    key: str
    escape_surface: ManifestSurface
    copy_dispose_surface: ManifestSurface


__all__ = [
    "CaptureFixtureFacts",
    "CaptureFixtureSpec",
    "DiagnosticBatch",
    "ManifestSurface",
    "SurfaceProfile",
]
