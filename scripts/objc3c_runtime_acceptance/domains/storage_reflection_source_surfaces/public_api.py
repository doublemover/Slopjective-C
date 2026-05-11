"""Storage/reflection source-surface public API."""

from __future__ import annotations

from typing import Any

from ...case_result import CaseResult
from .orchestration import build_storage_reflection_source_surface
from .source_surfaces import (
    PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE,
    PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE,
)


def build_runtime_property_ivar_storage_accessor_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return build_storage_reflection_source_surface(
        results,
        PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE,
    )


def build_runtime_property_atomicity_synthesis_reflection_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return build_storage_reflection_source_surface(
        results,
        PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE,
    )


__all__ = [
    "build_runtime_property_atomicity_synthesis_reflection_source_surface",
    "build_runtime_property_ivar_storage_accessor_source_surface",
]
