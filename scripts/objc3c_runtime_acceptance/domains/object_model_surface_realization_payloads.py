"""Object Model realization acceptance evidence payload construction."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from .object_model_surface_realization_catalog import (
    CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CASE_IDS,
    CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CONTRACT_IDS,
    CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_FIXTURES,
    CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_PROBES,
    DISPATCH_TABLE_LOWERING_MODEL,
    DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CASE_IDS,
    DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CONTRACT_IDS,
    DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_FIXTURES,
    DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_PROBES,
    LOWERING_ARTIFACT_BOUNDARY_MODEL,
    OBJECT_MODEL_REALIZATION_SOURCE_CASE_IDS,
    OBJECT_MODEL_REALIZATION_SOURCE_CONTRACT_IDS,
    OBJECT_MODEL_REALIZATION_SOURCE_FIXTURES,
    OBJECT_MODEL_REALIZATION_SOURCE_PROBES,
    PRIVATE_OBJECT_MODEL_QUERY_BOUNDARY,
    PRIVATE_REFLECTION_ARTIFACT_QUERY_BOUNDARY,
    REALIZATION_LOWERING_REFLECTION_ARTIFACT_CASE_IDS,
    REALIZATION_LOWERING_REFLECTION_ARTIFACT_CONTRACT_IDS,
    REALIZATION_LOWERING_REFLECTION_ARTIFACT_FIXTURES,
    REALIZATION_LOWERING_REFLECTION_ARTIFACT_PROBES,
    REALIZATION_PROVENANCE_COMPILE_ARTIFACTS,
    REALIZATION_STANDARD_COMPILE_ARTIFACTS,
    REALIZED_METADATA_REPLAY_COMPILE_ARTIFACTS,
    REALIZED_METADATA_REPLAY_PRESERVATION_MODEL,
    REFLECTION_ARTIFACT_HANDOFF_MODEL,
    REFLECTION_RECORD_LOWERING_MODEL,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
)
from .object_model_surface_realization_predicates import authoritative_case_ids


def runtime_object_model_realization_source_surface_payload(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": list(REALIZATION_STANDARD_COMPILE_ARTIFACTS),
        "source_contract_ids": list(OBJECT_MODEL_REALIZATION_SOURCE_CONTRACT_IDS),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": list(PRIVATE_OBJECT_MODEL_QUERY_BOUNDARY),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            OBJECT_MODEL_REALIZATION_SOURCE_CASE_IDS,
        ),
        "authoritative_fixture_paths": list(OBJECT_MODEL_REALIZATION_SOURCE_FIXTURES),
        "authoritative_probe_paths": list(OBJECT_MODEL_REALIZATION_SOURCE_PROBES),
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def runtime_realization_lowering_reflection_artifact_surface_payload(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_artifact_set": list(REALIZATION_PROVENANCE_COMPILE_ARTIFACTS),
        "source_contract_ids": list(
            REALIZATION_LOWERING_REFLECTION_ARTIFACT_CONTRACT_IDS
        ),
        "lowering_artifact_boundary_model": LOWERING_ARTIFACT_BOUNDARY_MODEL,
        "reflection_artifact_handoff_model": REFLECTION_ARTIFACT_HANDOFF_MODEL,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_reflection_artifact_query_boundary": list(
            PRIVATE_REFLECTION_ARTIFACT_QUERY_BOUNDARY
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            REALIZATION_LOWERING_REFLECTION_ARTIFACT_CASE_IDS,
        ),
        "authoritative_fixture_paths": list(
            REALIZATION_LOWERING_REFLECTION_ARTIFACT_FIXTURES
        ),
        "authoritative_probe_paths": list(
            REALIZATION_LOWERING_REFLECTION_ARTIFACT_PROBES
        ),
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def runtime_dispatch_table_reflection_record_lowering_surface_payload(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": list(REALIZATION_PROVENANCE_COMPILE_ARTIFACTS),
        "source_contract_ids": list(
            DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CONTRACT_IDS
        ),
        "dispatch_table_lowering_model": DISPATCH_TABLE_LOWERING_MODEL,
        "reflection_record_lowering_model": REFLECTION_RECORD_LOWERING_MODEL,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids(
            results,
            DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_CASE_IDS,
        ),
        "authoritative_fixture_paths": list(
            DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_FIXTURES
        ),
        "authoritative_probe_paths": list(
            DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_PROBES
        ),
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def runtime_cross_module_realized_metadata_replay_preservation_surface_payload(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": (
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": list(REALIZED_METADATA_REPLAY_COMPILE_ARTIFACTS),
        "source_contract_ids": list(
            CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CONTRACT_IDS
        ),
        "realized_metadata_replay_preservation_model": (
            REALIZED_METADATA_REPLAY_PRESERVATION_MODEL
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids(
            results,
            CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_CASE_IDS,
        ),
        "authoritative_fixture_paths": list(
            CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_FIXTURES
        ),
        "authoritative_probe_paths": list(
            CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_PROBES
        ),
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = [
    "runtime_cross_module_realized_metadata_replay_preservation_surface_payload",
    "runtime_dispatch_table_reflection_record_lowering_surface_payload",
    "runtime_object_model_realization_source_surface_payload",
    "runtime_realization_lowering_reflection_artifact_surface_payload",
]
