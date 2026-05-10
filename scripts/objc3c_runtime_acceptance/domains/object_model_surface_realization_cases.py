"""Object Model realization and lowering acceptance surface facade."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from .object_model_surface_realization_assertions import (
    expect_runtime_cross_module_realized_metadata_replay_preservation_surface,
    expect_runtime_dispatch_table_reflection_record_lowering_surface,
    expect_runtime_object_model_realization_source_surface,
    expect_runtime_realization_lowering_reflection_artifact_surface,
)
from .object_model_surface_realization_catalog import EXPORTED_CASE_NAMES
from .object_model_surface_realization_payloads import (
    runtime_cross_module_realized_metadata_replay_preservation_surface_payload,
    runtime_dispatch_table_reflection_record_lowering_surface_payload,
    runtime_object_model_realization_source_surface_payload,
    runtime_realization_lowering_reflection_artifact_surface_payload,
)

_EXPORTED_CASE_NAMES = list(EXPORTED_CASE_NAMES)


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def build_runtime_object_model_realization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    surface = runtime_object_model_realization_source_surface_payload(results)
    expect_runtime_object_model_realization_source_surface(surface)
    return surface


def build_runtime_realization_lowering_reflection_artifact_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    surface = runtime_realization_lowering_reflection_artifact_surface_payload(results)
    expect_runtime_realization_lowering_reflection_artifact_surface(surface)
    return surface


def build_runtime_dispatch_table_reflection_record_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    surface = runtime_dispatch_table_reflection_record_lowering_surface_payload(results)
    expect_runtime_dispatch_table_reflection_record_lowering_surface(surface)
    return surface


def build_runtime_cross_module_realized_metadata_replay_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    surface = (
        runtime_cross_module_realized_metadata_replay_preservation_surface_payload(
            results
        )
    )
    expect_runtime_cross_module_realized_metadata_replay_preservation_surface(surface)
    return surface


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
