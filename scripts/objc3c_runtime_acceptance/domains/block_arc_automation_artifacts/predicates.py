"""Manifest surface predicates for block/ARC storage automation artifacts."""

from __future__ import annotations

from .constants import SEMANTIC_SURFACE_PATH, SEMA_PASS_MANAGER_PATH
from .models import ManifestSurface


def manifest_surface_at_path(
    manifest: ManifestSurface,
    path: tuple[str, ...],
) -> ManifestSurface:
    surface = manifest
    for key in path:
        value = surface.get(key, {})
        if not isinstance(value, dict):
            return {}
        surface = value
    return surface


def semantic_surface(manifest: ManifestSurface) -> ManifestSurface:
    return manifest_surface_at_path(manifest, SEMANTIC_SURFACE_PATH)


def sema_surface(manifest: ManifestSurface) -> ManifestSurface:
    return manifest_surface_at_path(manifest, SEMA_PASS_MANAGER_PATH)


def semantic_surface_entry(
    manifest: ManifestSurface,
    key: str,
) -> ManifestSurface:
    value = semantic_surface(manifest).get(key, {})
    if isinstance(value, dict):
        return value
    return {}


__all__ = [
    "manifest_surface_at_path",
    "semantic_surface",
    "semantic_surface_entry",
    "sema_surface",
]
