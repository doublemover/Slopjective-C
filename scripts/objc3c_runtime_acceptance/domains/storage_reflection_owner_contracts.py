"""Source-owned storage/reflection runtime acceptance owner contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


STORAGE_REFLECTION_OWNER_CONTRACT_ID = (
    "objc3c.runtime.storage.reflection.owner.contract.v1"
)
STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID = (
    "objc3c.runtime.storage.reflection.strict.status.owner.v1"
)

SEMANTIC_SYNTHESIS_OWNER = "storage-reflection.semantic-synthesis"
LOWERING_METADATA_OWNER = "storage-reflection.lowering-metadata"
IVAR_LAYOUT_ACCESSOR_OWNER = "storage-reflection.ivar-layout-accessor"
RUNTIME_PROPERTY_REFLECTION_OWNER = "storage-reflection.runtime-property-reflection"
RUNTIME_PROPERTY_EXECUTION_OWNER = "storage-reflection.runtime-property-execution"
OWNERSHIP_ARC_OWNER = "storage-reflection.ownership-arc"
CROSS_MODULE_ARTIFACT_OWNER = "storage-reflection.cross-module-artifact"
STRICT_STATUS_OWNER = "storage-reflection.strict-status"

STORAGE_REFLECTION_OWNER_CASE_IDS: tuple[str, ...] = (
    "property-synthesis-storage-binding-semantics",
    "property-reflection-accessor-compatibility-diagnostics",
    "storage-legality-semantics",
    "property-ivar-ordering-semantics",
    "accessor-storage-lowering-metadata-surface",
    "property-accessor-layout-lowering",
    "synthesized-accessor-codegen",
    "synthesized-accessor-runtime",
    "property-layout",
    "instance-allocation-layout-runtime",
    "property-reflection",
    "property-execution",
    "storage-ownership-reflection",
    "cross-module-storage-reflection-artifact-preservation",
)
STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS: tuple[str, ...] = tuple(
    case_id
    for case_id in STORAGE_REFLECTION_OWNER_CASE_IDS
    if case_id != "cross-module-storage-reflection-artifact-preservation"
)

STORAGE_REFLECTION_REQUIRED_OWNER_IDS: tuple[str, ...] = (
    SEMANTIC_SYNTHESIS_OWNER,
    LOWERING_METADATA_OWNER,
    IVAR_LAYOUT_ACCESSOR_OWNER,
    RUNTIME_PROPERTY_REFLECTION_OWNER,
    RUNTIME_PROPERTY_EXECUTION_OWNER,
    OWNERSHIP_ARC_OWNER,
    CROSS_MODULE_ARTIFACT_OWNER,
    STRICT_STATUS_OWNER,
)


@dataclass(frozen=True)
class StorageReflectionOwnerContract:
    owner_id: str
    owner_surface: str
    case_ids: tuple[str, ...]
    source_modules: tuple[str, ...]
    owned_decisions: tuple[str, ...]

    def payload(self) -> dict[str, Any]:
        return {
            "contract_id": STORAGE_REFLECTION_OWNER_CONTRACT_ID,
            "owner_id": self.owner_id,
            "owner_surface": self.owner_surface,
            "case_ids": list(self.case_ids),
            "source_modules": list(self.source_modules),
            "owned_decisions": list(self.owned_decisions),
        }


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
    case_ids=("property-reflection",),
    source_modules=(
        "storage_reflection_runtime_property_reflection_cases",
        "storage_reflection_runtime_property_reflection_summary",
        "storage_reflection_runtime_property_reflection_assertions",
    ),
    owned_decisions=(
        "property registry reflection payload",
        "slot-backed property reflection counts",
        "runtime setter reflection availability",
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


def storage_reflection_owner_contract_payloads() -> list[dict[str, Any]]:
    return [contract.payload() for contract in STORAGE_REFLECTION_OWNER_CONTRACTS]


def storage_reflection_strict_status_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
        "owner_id": STRICT_STATUS_OWNER,
        "owner_surface": "storage-reflection runtime acceptance status",
        "case_ids": list(STORAGE_REFLECTION_OWNER_CASE_IDS),
        "workflow_actions": [
            "validate-storage-reflection-conformance",
            "validate-runnable-storage-reflection",
        ],
        "status_policy": (
            "case status is derived from CaseResult pass/fail data and missing "
            "owner coverage raises immediately"
        ),
    }


def storage_reflection_case_owner_payload(case_id: str) -> dict[str, Any]:
    contract = _CASE_OWNER_CONTRACTS.get(case_id)
    if contract is None:
        raise ValueError(f"unowned storage/reflection runtime acceptance case: {case_id}")
    payload = contract.payload()
    payload["strict_status_owner"] = storage_reflection_strict_status_owner_payload()
    return payload


def storage_reflection_case_summary(
    case_id: str,
    summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "owner_contract": storage_reflection_case_owner_payload(case_id),
        **summary,
    }


def storage_reflection_surface_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": STORAGE_REFLECTION_OWNER_CONTRACT_ID,
        "owner_ids": list(STORAGE_REFLECTION_REQUIRED_OWNER_IDS),
        "owner_contracts": storage_reflection_owner_contract_payloads(),
        "strict_status_owner": storage_reflection_strict_status_owner_payload(),
    }


def assert_storage_reflection_owner_case_ids(case_ids: tuple[str, ...]) -> None:
    expected = set(STORAGE_REFLECTION_OWNER_CASE_IDS)
    actual = set(case_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "storage/reflection owner case inventory drift: "
            f"missing={missing} extra={extra}"
        )


def assert_storage_reflection_direct_factory_case_ids(
    case_ids: tuple[str, ...],
) -> None:
    expected = set(STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS)
    actual = set(case_ids)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        raise ValueError(
            "storage/reflection direct factory owner drift: "
            f"missing={missing} extra={extra}"
        )


__all__ = [
    "CROSS_MODULE_ARTIFACT_OWNER",
    "CROSS_MODULE_ARTIFACT_OWNER_CONTRACT",
    "IVAR_LAYOUT_ACCESSOR_OWNER",
    "IVAR_LAYOUT_ACCESSOR_OWNER_CONTRACT",
    "LOWERING_METADATA_OWNER",
    "LOWERING_METADATA_OWNER_CONTRACT",
    "OWNERSHIP_ARC_OWNER",
    "OWNERSHIP_ARC_OWNER_CONTRACT",
    "RUNTIME_PROPERTY_EXECUTION_OWNER",
    "RUNTIME_PROPERTY_EXECUTION_OWNER_CONTRACT",
    "RUNTIME_PROPERTY_REFLECTION_OWNER",
    "RUNTIME_PROPERTY_REFLECTION_OWNER_CONTRACT",
    "SEMANTIC_SYNTHESIS_OWNER",
    "SEMANTIC_SYNTHESIS_OWNER_CONTRACT",
    "STORAGE_REFLECTION_OWNER_CASE_IDS",
    "STORAGE_REFLECTION_OWNER_CONTRACTS",
    "STORAGE_REFLECTION_OWNER_CONTRACT_ID",
    "STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS",
    "STORAGE_REFLECTION_REQUIRED_OWNER_IDS",
    "STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID",
    "STRICT_STATUS_OWNER",
    "StorageReflectionOwnerContract",
    "assert_storage_reflection_direct_factory_case_ids",
    "assert_storage_reflection_owner_case_ids",
    "storage_reflection_case_owner_payload",
    "storage_reflection_case_summary",
    "storage_reflection_owner_contract_payloads",
    "storage_reflection_strict_status_owner_payload",
    "storage_reflection_surface_owner_payload",
]
