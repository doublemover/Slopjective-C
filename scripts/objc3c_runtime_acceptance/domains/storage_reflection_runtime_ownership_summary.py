"""Summary builders for storage ownership reflection runtime cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_payload_assertions import (
    StorageOwnershipReflectionFacts,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_sources import (
    StorageOwnershipReflectionArtifacts,
    StorageOwnershipReflectionSources,
)
from .storage_reflection_owner_contracts import storage_reflection_case_summary


STORAGE_OWNERSHIP_REFLECTION_CASE_ID = "storage-ownership-reflection"


def build_storage_ownership_reflection_result(
    sources: StorageOwnershipReflectionSources,
    artifacts: StorageOwnershipReflectionArtifacts,
    facts: StorageOwnershipReflectionFacts,
) -> CaseResult:
    return CaseResult(
        case_id=sources.case_id,
        probe=sources.probe_summary_path(),
        fixture=sources.fixture_summary_path(),
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_storage_ownership_reflection_summary(artifacts, facts),
    )


def build_storage_ownership_reflection_summary(
    artifacts: StorageOwnershipReflectionArtifacts,
    facts: StorageOwnershipReflectionFacts,
) -> dict[str, Any]:
    return storage_reflection_case_summary(
        STORAGE_OWNERSHIP_REFLECTION_CASE_ID,
        {
            "runtime_property_accessor_count": facts.box_entry.get(
                "runtime_property_accessor_count"
            ),
            "runtime_instance_size_bytes": facts.box_entry.get(
                "runtime_instance_size_bytes"
            ),
            "property_descriptor_count": artifacts.registration_manifest.get(
                "property_descriptor_count"
            ),
            "ivar_descriptor_count": artifacts.registration_manifest.get(
                "ivar_descriptor_count"
            ),
            "implementation_surface_contract_id": (
                facts.manifest_implementation_surface.get("contract_id")
            ),
            "implementation_snapshot_symbol": facts.manifest_implementation_surface.get(
                "implementation_snapshot_symbol"
            ),
            "guarded_runtime_hook_profile": facts.payload.get(
                "guarded_value_property",
                {},
            ).get("ownership_runtime_hook_profile"),
            "weak_runtime_hook_profile": facts.payload.get(
                "weak_value_property",
                {},
            ).get("ownership_runtime_hook_profile"),
        },
    )


__all__ = [
    "build_storage_ownership_reflection_result",
    "build_storage_ownership_reflection_summary",
    "STORAGE_OWNERSHIP_REFLECTION_CASE_ID",
]
