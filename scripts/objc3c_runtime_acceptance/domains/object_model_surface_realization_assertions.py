"""Object Model realization acceptance surface assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from .object_model_surface_realization_catalog import (
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
)


def expect_realization_surface_contract(
    surface: dict[str, Any],
    expected_contract_id: str,
    surface_label: str,
) -> None:
    expect(
        surface.get("contract_id") == expected_contract_id,
        f"expected {surface_label} to publish contract id {expected_contract_id}",
    )
    expect(
        isinstance(surface.get("compile_artifact_set"), list),
        f"expected {surface_label} to publish compile artifacts as a list",
    )
    expect(
        isinstance(surface.get("source_contract_ids"), list),
        f"expected {surface_label} to publish source contract ids as a list",
    )
    expect(
        isinstance(surface.get("authoritative_case_ids"), list),
        f"expected {surface_label} to publish authoritative case ids as a list",
    )


def expect_runtime_object_model_realization_source_surface(
    surface: dict[str, Any],
) -> None:
    expect_realization_surface_contract(
        surface,
        RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "runtime object model realization source surface",
    )


def expect_runtime_realization_lowering_reflection_artifact_surface(
    surface: dict[str, Any],
) -> None:
    expect_realization_surface_contract(
        surface,
        RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "runtime realization lowering reflection artifact surface",
    )


def expect_runtime_dispatch_table_reflection_record_lowering_surface(
    surface: dict[str, Any],
) -> None:
    expect_realization_surface_contract(
        surface,
        RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "runtime dispatch table reflection record lowering surface",
    )


def expect_runtime_cross_module_realized_metadata_replay_preservation_surface(
    surface: dict[str, Any],
) -> None:
    expect_realization_surface_contract(
        surface,
        RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "runtime cross-module realized metadata replay preservation surface",
    )


__all__ = [
    "expect_realization_surface_contract",
    "expect_runtime_cross_module_realized_metadata_replay_preservation_surface",
    "expect_runtime_dispatch_table_reflection_record_lowering_surface",
    "expect_runtime_object_model_realization_source_surface",
    "expect_runtime_realization_lowering_reflection_artifact_surface",
]
