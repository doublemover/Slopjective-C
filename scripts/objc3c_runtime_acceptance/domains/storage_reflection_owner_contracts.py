"""Source-owned storage/reflection runtime acceptance owner contracts."""

from __future__ import annotations

from .storage_reflection_owner_contract_catalog import (
    CROSS_MODULE_ARTIFACT_OWNER_CONTRACT,
    IVAR_LAYOUT_ACCESSOR_OWNER_CONTRACT,
    LOWERING_METADATA_OWNER_CONTRACT,
    OWNERSHIP_ARC_OWNER_CONTRACT,
    RUNTIME_PROPERTY_EXECUTION_OWNER_CONTRACT,
    RUNTIME_PROPERTY_REFLECTION_OWNER_CONTRACT,
    SEMANTIC_SYNTHESIS_OWNER_CONTRACT,
    STORAGE_REFLECTION_OWNER_CONTRACTS,
    _CASE_OWNER_CONTRACTS,
)
from .storage_reflection_owner_contract_inventory import (
    CROSS_MODULE_ARTIFACT_OWNER,
    IVAR_LAYOUT_ACCESSOR_OWNER,
    LOWERING_METADATA_OWNER,
    OWNERSHIP_ARC_OWNER,
    RUNTIME_PROPERTY_EXECUTION_OWNER,
    RUNTIME_PROPERTY_REFLECTION_OWNER,
    SEMANTIC_SYNTHESIS_OWNER,
    STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CONTRACT_ID,
    STORAGE_REFLECTION_REQUIRED_OWNER_IDS,
    STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
    STRICT_STATUS_OWNER,
)
from .storage_reflection_owner_contract_model import StorageReflectionOwnerContract
from .storage_reflection_owner_contract_payloads import (
    assert_storage_reflection_direct_factory_case_ids,
    assert_storage_reflection_owner_case_ids,
    storage_reflection_case_owner_payload,
    storage_reflection_case_summary,
    storage_reflection_owner_contract_payloads,
    storage_reflection_strict_status_owner_payload,
    storage_reflection_surface_owner_payload,
)


StorageReflectionOwnerContract.__module__ = __name__


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
