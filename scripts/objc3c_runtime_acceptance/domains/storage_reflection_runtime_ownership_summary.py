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
OWNERSHIP_PROPERTY_KEYS = (
    "current_value_property",
    "copied_value_property",
    "weak_value_property",
    "borrowed_value_property",
    "guarded_value_property",
)


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
    current_value = facts.payload.get("current_value_property", {})
    weak_value = facts.payload.get("weak_value_property", {})
    guarded_value = facts.payload.get("guarded_value_property", {})
    reflected_property_count = sum(
        1
        for key in OWNERSHIP_PROPERTY_KEYS
        if facts.payload.get(key, {}).get("found") == 1
    )
    return storage_reflection_case_summary(
        STORAGE_OWNERSHIP_REFLECTION_CASE_ID,
        {
            "owned_value": _ownership_property_replay_summary(current_value),
            "weak_value": _ownership_property_replay_summary(weak_value),
            "reflected_property_count": reflected_property_count,
            "ownership_runtime_hook_profile": weak_value.get(
                "ownership_runtime_hook_profile"
            ),
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
            "guarded_runtime_hook_profile": guarded_value.get(
                "ownership_runtime_hook_profile"
            ),
            "weak_runtime_hook_profile": weak_value.get(
                "ownership_runtime_hook_profile"
            ),
        },
    )


def _ownership_property_replay_summary(property_payload: dict[str, Any]) -> dict[str, Any]:
    return {
        "property_name": property_payload.get("property_name"),
        "slot_index": property_payload.get("slot_index"),
        "ownership_lifetime_profile": property_payload.get(
            "ownership_lifetime_profile"
        ),
        "ownership_runtime_hook_profile": property_payload.get(
            "ownership_runtime_hook_profile"
        ),
        "has_runtime_getter": property_payload.get("has_runtime_getter"),
        "has_runtime_setter": property_payload.get("has_runtime_setter"),
    }


__all__ = [
    "build_storage_ownership_reflection_result",
    "build_storage_ownership_reflection_summary",
    "STORAGE_OWNERSHIP_REFLECTION_CASE_ID",
]
