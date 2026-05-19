"""Storage and reflection runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_cross_module_cases import (
    check_cross_module_storage_reflection_artifact_preservation_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_cases import (
    check_accessor_storage_lowering_metadata_surface_case,
    check_property_accessor_layout_lowering_case,
    check_synthesized_accessor_codegen_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_owner_contracts import (
    assert_storage_reflection_owner_case_ids,
    storage_reflection_case_owner_payload,
    storage_reflection_case_summary,
    storage_reflection_owner_contract_payloads,
    storage_reflection_strict_status_owner_payload,
    storage_reflection_surface_owner_payload,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_cases import (
    check_instance_allocation_layout_runtime_case,
    check_property_execution_case,
    check_property_invalid_layout_runtime_case,
    check_property_layout_case,
    check_property_reflection_case,
    check_storage_ownership_reflection_case,
    check_synthesized_accessor_runtime_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_semantic_cases import (
    check_property_ivar_ordering_semantics_case,
    check_property_reflection_accessor_compatibility_diagnostics_case,
    check_property_synthesis_storage_binding_semantics_case,
    check_storage_legality_semantics_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_surfaces import (
    build_dispatch_and_synthesized_accessor_lowering_surface,
    build_executable_ivar_layout_emission_surface,
    build_executable_property_accessor_layout_lowering_surface,
    build_executable_synthesized_accessor_property_lowering_surface,
    build_runtime_property_atomicity_synthesis_reflection_source_surface,
    build_runtime_property_ivar_storage_accessor_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "assert_storage_reflection_owner_case_ids",
    "storage_reflection_case_owner_payload",
    "storage_reflection_case_summary",
    "storage_reflection_owner_contract_payloads",
    "storage_reflection_strict_status_owner_payload",
    "storage_reflection_surface_owner_payload",
    "build_runtime_property_ivar_storage_accessor_source_surface",
    "build_runtime_property_atomicity_synthesis_reflection_source_surface",
    "build_dispatch_and_synthesized_accessor_lowering_surface",
    "build_executable_property_accessor_layout_lowering_surface",
    "build_executable_ivar_layout_emission_surface",
    "build_executable_synthesized_accessor_property_lowering_surface",
    "check_accessor_storage_lowering_metadata_surface_case",
    "check_property_ivar_ordering_semantics_case",
    "check_property_reflection_accessor_compatibility_diagnostics_case",
    "check_property_synthesis_storage_binding_semantics_case",
    "check_storage_legality_semantics_case",
    "check_synthesized_accessor_codegen_case",
    "check_synthesized_accessor_runtime_case",
    "check_property_accessor_layout_lowering_case",
    "check_property_layout_case",
    "check_instance_allocation_layout_runtime_case",
    "check_property_execution_case",
    "check_property_reflection_case",
    "check_property_invalid_layout_runtime_case",
    "check_storage_ownership_reflection_case",
    "check_cross_module_storage_reflection_artifact_preservation_case",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
