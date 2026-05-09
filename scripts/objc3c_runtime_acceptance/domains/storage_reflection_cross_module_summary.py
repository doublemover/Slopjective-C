"""Summary helpers for cross-module storage/reflection preservation cases."""

from __future__ import annotations

from typing import Any

from ..runtime_contract_storage_reflection import (
    STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
    STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
)
from .storage_reflection_cross_module_artifacts import (
    CrossModuleStorageReflectionArtifacts,
)


def build_cross_module_storage_reflection_summary(
    artifacts: CrossModuleStorageReflectionArtifacts,
    case_total_ms: int,
) -> dict[str, Any]:
    return {
        "provider_fixture": STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
        "consumer_fixture": STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
        "provider_compile_ms": artifacts.provider_compile_ms,
        "consumer_compile_ms": artifacts.consumer_compile_ms,
        "case_total_ms": case_total_ms,
        "provider_module_name": artifacts.provider_import_payload.get("module_name"),
        "consumer_module_name": artifacts.link_plan.get("local_module", {}).get("module_name"),
        "imported_property_descriptor_count": artifacts.link_plan.get(
            "imported_property_descriptor_count"
        ),
        "imported_ivar_descriptor_count": artifacts.link_plan.get(
            "imported_ivar_descriptor_count"
        ),
        "imported_synthesized_accessor_entries": artifacts.link_plan.get(
            "imported_storage_reflection_synthesized_accessor_entries"
        ),
        "imported_ivar_layout_entries": artifacts.link_plan.get(
            "imported_storage_reflection_ivar_layout_entries"
        ),
    }


__all__ = ["build_cross_module_storage_reflection_summary"]
