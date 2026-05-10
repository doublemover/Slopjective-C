"""Storage/reflection semantic legality helper predicates."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from ..runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)
from .storage_reflection_semantic_legality_catalog import (
    EXPECTED_STORAGE_LEGALITY_IVAR_DESCRIPTOR_COUNT,
    EXPECTED_STORAGE_LEGALITY_PROPERTY_DESCRIPTOR_COUNT,
)


def read_json_object(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def semantic_pass_manager_manifest(manifest: dict[str, Any]) -> dict[str, Any]:
    return manifest.get("frontend", {}).get("pipeline", {}).get("sema_pass_manager", {})


def has_storage_legality_property_descriptor_count(
    registration_manifest: dict[str, Any],
) -> bool:
    return (
        registration_manifest.get("property_descriptor_count")
        == EXPECTED_STORAGE_LEGALITY_PROPERTY_DESCRIPTOR_COUNT
    )


def has_storage_legality_ivar_descriptor_count(
    registration_manifest: dict[str, Any],
) -> bool:
    return (
        registration_manifest.get("ivar_descriptor_count")
        == EXPECTED_STORAGE_LEGALITY_IVAR_DESCRIPTOR_COUNT
    )


def has_property_ivar_storage_accessor_source_contract(
    manifest: dict[str, Any],
) -> bool:
    return (
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
    )


def has_property_atomicity_synthesis_reflection_source_contract(
    manifest: dict[str, Any],
) -> bool:
    return (
        manifest.get(
            "runtime_property_atomicity_synthesis_reflection_source_surface", {}
        ).get("contract_id")
        == RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
    )


def runtime_export_boundary_is_ready(sema_pass_manager_manifest: dict[str, Any]) -> bool:
    return sema_pass_manager_manifest.get("runtime_export_boundary_ready") is True


def runtime_export_counter_is_zero(
    sema_pass_manager_manifest: dict[str, Any],
    counter_name: str,
) -> bool:
    return sema_pass_manager_manifest.get(counter_name) == 0


def ll_text_contains_storage_legality_evidence(ll_text: str, needle: str) -> bool:
    return needle in ll_text


def diagnostic_results_by_key(
    negative_batch: dict[str, Any],
) -> dict[str, dict[str, Any]]:
    return {str(entry["key"]): entry for entry in negative_batch["results"]}
