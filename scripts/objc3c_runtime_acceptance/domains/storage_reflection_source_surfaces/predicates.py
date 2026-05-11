"""Storage/reflection expected runtime and storage predicates."""

from __future__ import annotations

from collections.abc import Iterable

from ...case_result import CaseResult
from .case_catalog import (
    PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_CASE_IDS,
    PROPERTY_IVAR_STORAGE_ACCESSOR_CASE_IDS,
)
from .models import StorageReflectionSourceSurfaceDefinition


def is_expected_property_ivar_storage_case_result(result: CaseResult) -> bool:
    return result.case_id in PROPERTY_IVAR_STORAGE_ACCESSOR_CASE_IDS


def is_expected_property_atomicity_runtime_case_result(result: CaseResult) -> bool:
    return result.case_id in PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_CASE_IDS


def is_authoritative_source_surface_case_result(
    result: CaseResult,
    surface: StorageReflectionSourceSurfaceDefinition,
) -> bool:
    return result.case_id in surface.case_ids


def authoritative_case_ids(
    results: Iterable[CaseResult],
    surface: StorageReflectionSourceSurfaceDefinition,
) -> list[str]:
    return [
        result.case_id
        for result in results
        if is_authoritative_source_surface_case_result(result, surface)
    ]


__all__ = [
    "authoritative_case_ids",
    "is_authoritative_source_surface_case_result",
    "is_expected_property_atomicity_runtime_case_result",
    "is_expected_property_ivar_storage_case_result",
]
