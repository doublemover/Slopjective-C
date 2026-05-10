"""Storage/reflection owner IDs and runtime acceptance case inventories."""

from __future__ import annotations


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


__all__ = [
    "CROSS_MODULE_ARTIFACT_OWNER",
    "IVAR_LAYOUT_ACCESSOR_OWNER",
    "LOWERING_METADATA_OWNER",
    "OWNERSHIP_ARC_OWNER",
    "RUNTIME_PROPERTY_EXECUTION_OWNER",
    "RUNTIME_PROPERTY_REFLECTION_OWNER",
    "SEMANTIC_SYNTHESIS_OWNER",
    "STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS",
    "STORAGE_REFLECTION_OWNER_CASE_IDS",
    "STORAGE_REFLECTION_OWNER_CONTRACT_ID",
    "STORAGE_REFLECTION_REQUIRED_OWNER_IDS",
    "STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID",
    "STRICT_STATUS_OWNER",
]
