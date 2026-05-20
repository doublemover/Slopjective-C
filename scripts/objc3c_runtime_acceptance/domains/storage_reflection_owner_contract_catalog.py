"""Storage/reflection concrete owner contract catalog."""

from __future__ import annotations

from .storage_reflection_owner_contract_inventory import (
    CROSS_MODULE_ARTIFACT_OWNER,
    IVAR_LAYOUT_ACCESSOR_OWNER,
    LOWERING_METADATA_OWNER,
    OWNERSHIP_ARC_OWNER,
    RUNTIME_PROPERTY_EXECUTION_OWNER,
    RUNTIME_PROPERTY_REFLECTION_OWNER,
    SEMANTIC_SYNTHESIS_OWNER,
)
from .storage_reflection_owner_contract_model import StorageReflectionOwnerContract


SEMANTIC_SYNTHESIS_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=SEMANTIC_SYNTHESIS_OWNER,
    owner_surface="semantic synthesis, diagnostics, legality, and source ordering",
    case_ids=(
        "property-synthesis-storage-binding-semantics",
        "property-reflection-accessor-compatibility-diagnostics",
        "storage-legality-semantics",
        "property-ivar-ordering-semantics",
    ),
    source_modules=(
        "storage_reflection_semantic_cases",
        "storage_reflection_semantic_synthesis_cases",
        "storage_reflection_semantic_diagnostic_cases",
        "storage_reflection_semantic_legality_cases",
        "storage_reflection_semantic_ordering_cases",
    ),
    owned_decisions=(
        "property synthesis source binding",
        "declaration-level accessor compatibility",
        "storage legality diagnostics",
        "ivar ordering source records",
    ),
)

LOWERING_METADATA_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=LOWERING_METADATA_OWNER,
    owner_surface="lowering metadata and helper selection",
    case_ids=("accessor-storage-lowering-metadata-surface",),
    source_modules=(
        "storage_reflection_lowering_metadata_cases",
        "storage_reflection_lowering_metadata_summary",
        "storage_reflection_lowering_metadata_surface_assertions",
    ),
    owned_decisions=(
        "lowering metadata contract publication",
        "synthesized accessor helper selection",
        "ARC helper lowering metadata",
    ),
)

IVAR_LAYOUT_ACCESSOR_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=IVAR_LAYOUT_ACCESSOR_OWNER,
    owner_surface="ivar layout, accessor layout, synthesized codegen, and layout runtime",
    case_ids=(
        "property-accessor-layout-lowering",
        "synthesized-accessor-codegen",
        "synthesized-accessor-runtime",
        "property-layout",
        "instance-allocation-layout-runtime",
    ),
    source_modules=(
        "storage_reflection_lowering_layout_cases",
        "storage_reflection_lowering_layout_accessor_case",
        "storage_reflection_lowering_layout_codegen_case",
        "storage_reflection_runtime_accessor_cases",
        "storage_reflection_runtime_layout_cases",
    ),
    owned_decisions=(
        "property descriptor layout lowering",
        "ivar descriptor layout lowering",
        "synthesized accessor codegen identity",
        "linked runtime layout probes",
    ),
)

RUNTIME_PROPERTY_REFLECTION_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=RUNTIME_PROPERTY_REFLECTION_OWNER,
    owner_surface="runtime property metadata reflection",
    case_ids=(
        "property-reflection",
        "property-ivar-invalid-layout-runtime",
    ),
    source_modules=(
        "storage_reflection_runtime_property_reflection_cases",
        "storage_reflection_runtime_property_reflection_summary",
        "storage_reflection_runtime_property_reflection_assertions",
        "storage_reflection_runtime_property_invalid_layout_cases",
        "storage_reflection_runtime_property_invalid_layout_summary",
        "storage_reflection_runtime_property_invalid_layout_assertions",
    ),
    owned_decisions=(
        "property registry reflection payload",
        "slot-backed property reflection counts",
        "runtime setter reflection availability",
        "fail-closed invalid property/ivar layout reflection",
    ),
)

RUNTIME_PROPERTY_EXECUTION_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=RUNTIME_PROPERTY_EXECUTION_OWNER,
    owner_surface="runtime property execution and dispatch",
    case_ids=("property-execution",),
    source_modules=(
        "storage_reflection_runtime_property_execution_cases",
        "storage_reflection_runtime_property_execution_summary",
        "storage_reflection_runtime_property_execution_dispatch_assertions",
        "storage_reflection_runtime_property_execution_runtime_assertions",
    ),
    owned_decisions=(
        "runtime property accessor execution payload",
        "current-property dispatch identity",
        "slot-backed runtime state mutation",
    ),
)

OWNERSHIP_ARC_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=OWNERSHIP_ARC_OWNER,
    owner_surface="storage ownership ARC reflection",
    case_ids=("storage-ownership-reflection",),
    source_modules=(
        "storage_reflection_runtime_ownership_cases",
        "storage_reflection_runtime_ownership_summary",
        "storage_reflection_runtime_ownership_arc_assertions",
        "storage_reflection_runtime_ownership_payload_assertions",
    ),
    owned_decisions=(
        "strong/copy/weak/borrowed/guarded ownership profile reflection",
        "runtime ownership hook profile assertions",
        "implementation surface snapshot identity",
    ),
)

CROSS_MODULE_ARTIFACT_OWNER_CONTRACT = StorageReflectionOwnerContract(
    owner_id=CROSS_MODULE_ARTIFACT_OWNER,
    owner_surface="cross-module storage/reflection artifact preservation",
    case_ids=("cross-module-storage-reflection-artifact-preservation",),
    source_modules=(
        "storage_reflection_cross_module_cases",
        "storage_reflection_cross_module_summary",
        "storage_reflection_cross_module_artifacts",
        "storage_reflection_cross_module_assertions",
    ),
    owned_decisions=(
        "provider storage/reflection packet preservation",
        "consumer link plan import preservation",
        "transitive owner entry counts",
    ),
)

STORAGE_REFLECTION_OWNER_CONTRACTS: tuple[StorageReflectionOwnerContract, ...] = (
    SEMANTIC_SYNTHESIS_OWNER_CONTRACT,
    LOWERING_METADATA_OWNER_CONTRACT,
    IVAR_LAYOUT_ACCESSOR_OWNER_CONTRACT,
    RUNTIME_PROPERTY_REFLECTION_OWNER_CONTRACT,
    RUNTIME_PROPERTY_EXECUTION_OWNER_CONTRACT,
    OWNERSHIP_ARC_OWNER_CONTRACT,
    CROSS_MODULE_ARTIFACT_OWNER_CONTRACT,
)

_CASE_OWNER_CONTRACTS = {
    case_id: contract
    for contract in STORAGE_REFLECTION_OWNER_CONTRACTS
    for case_id in contract.case_ids
}


__all__ = [
    "CROSS_MODULE_ARTIFACT_OWNER_CONTRACT",
    "IVAR_LAYOUT_ACCESSOR_OWNER_CONTRACT",
    "LOWERING_METADATA_OWNER_CONTRACT",
    "OWNERSHIP_ARC_OWNER_CONTRACT",
    "RUNTIME_PROPERTY_EXECUTION_OWNER_CONTRACT",
    "RUNTIME_PROPERTY_REFLECTION_OWNER_CONTRACT",
    "SEMANTIC_SYNTHESIS_OWNER_CONTRACT",
    "STORAGE_REFLECTION_OWNER_CONTRACTS",
]
