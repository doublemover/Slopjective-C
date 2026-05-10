"""Storage/reflection layout lowering contract payload shaping."""

from __future__ import annotations

from typing import Any

from .catalog import ACCESSOR_LAYOUT_SURFACE_KEY
from .catalog import IVAR_LAYOUT_SURFACE_KEY
from .catalog import PROPERTY_SOURCE_SURFACE_KEY
from .catalog import SYNTHESIZED_ACCESSOR_SURFACE_KEY
from .data import ManifestSurface


def property_source_surface_payload(manifest: ManifestSurface) -> Any:
    return manifest.get(PROPERTY_SOURCE_SURFACE_KEY, {})


def accessor_layout_surface_payload(manifest: ManifestSurface) -> Any:
    return manifest.get(ACCESSOR_LAYOUT_SURFACE_KEY, {})


def ivar_layout_surface_payload(manifest: ManifestSurface) -> Any:
    return manifest.get(IVAR_LAYOUT_SURFACE_KEY, {})


def synthesized_accessor_surface_payload(manifest: ManifestSurface) -> Any:
    return manifest.get(SYNTHESIZED_ACCESSOR_SURFACE_KEY, {})
