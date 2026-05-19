"""Block/ARC capture legality result predicates."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any

from .data import SurfaceProfile


def has_escape_profile(
    surface: Mapping[str, Any],
    profile: SurfaceProfile,
) -> bool:
    return (
        surface.get("escape_to_heap_sites") == profile.escape_to_heap_sites
        and surface.get("requires_byref_cells_sites")
        == profile.requires_byref_cells_sites
    )


def has_copy_dispose_profile(
    surface: Mapping[str, Any],
    profile: SurfaceProfile,
) -> bool:
    return (
        surface.get("copy_helper_required_sites")
        == profile.copy_helper_required_sites
        and surface.get("dispose_helper_required_sites")
        == profile.dispose_helper_required_sites
    )


__all__ = [
    "has_copy_dispose_profile",
    "has_escape_profile",
]
