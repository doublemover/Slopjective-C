"""Ordered aggregate of runtime acceptance catalog surfaces."""

from __future__ import annotations

from ..model import SurfaceRequirement
from .foundation_surfaces import FOUNDATION_SURFACES
from .error_surfaces import ERROR_SURFACES
from .object_storage_surfaces import OBJECT_STORAGE_SURFACES
from .release_claim_surfaces import RELEASE_CLAIM_SURFACES
from .metaprogramming_surfaces import METAPROGRAMMING_SURFACES
from .interop_surfaces import INTEROP_SURFACES
from .reflection_surfaces import REFLECTION_SURFACES


COMMON_SURFACES: tuple[SurfaceRequirement, ...] = (
    *FOUNDATION_SURFACES,
    *ERROR_SURFACES,
    *OBJECT_STORAGE_SURFACES,
    *RELEASE_CLAIM_SURFACES,
    *METAPROGRAMMING_SURFACES,
    *INTEROP_SURFACES,
    *REFLECTION_SURFACES,
)

__all__ = ["COMMON_SURFACES"]
