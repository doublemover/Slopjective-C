"""Block ARC runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.block_arc_automation_cases import (
    check_block_storage_arc_automation_semantics_case,
)
from objc3c_runtime_acceptance.domains.block_arc_capture_cases import (
    check_escaping_block_capture_legality_case,
)
from objc3c_runtime_acceptance.domains.block_arc_cross_module_artifact_cases import (
    check_cross_module_block_ownership_artifact_preservation_case,
)
from objc3c_runtime_acceptance.domains.block_arc_property_cases import (
    check_arc_property_helper_case,
)
from objc3c_runtime_acceptance.domains.block_arc_runtime_cases import (
    check_block_arc_runtime_abi_case,
    check_block_helper_runtime_execution_case,
)
from objc3c_runtime_acceptance.domains.block_arc_surfaces import (
    build_runtime_block_arc_lowering_helper_surface,
    build_runtime_block_arc_runtime_abi_surface,
    build_runtime_block_arc_unified_source_surface,
    build_runtime_ownership_transfer_capture_family_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_block_arc_unified_source_surface",
    "build_runtime_ownership_transfer_capture_family_source_surface",
    "build_runtime_block_arc_lowering_helper_surface",
    "build_runtime_block_arc_runtime_abi_surface",
    "check_escaping_block_capture_legality_case",
    "check_block_storage_arc_automation_semantics_case",
    "check_block_arc_runtime_abi_case",
    "check_block_helper_runtime_execution_case",
    "check_arc_property_helper_case",
    "check_cross_module_block_ownership_artifact_preservation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
