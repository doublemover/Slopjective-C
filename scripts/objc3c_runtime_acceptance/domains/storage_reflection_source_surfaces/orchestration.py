"""Storage/reflection source-surface orchestration."""

from __future__ import annotations

from typing import Any

from ...case_result import CaseResult
from .models import StorageReflectionSourceSurfaceDefinition
from .payloads import storage_reflection_source_surface_payload


def build_storage_reflection_source_surface(
    results: list[CaseResult],
    surface: StorageReflectionSourceSurfaceDefinition,
) -> dict[str, Any]:
    return storage_reflection_source_surface_payload(results, surface)


__all__ = [
    "build_storage_reflection_source_surface",
]
