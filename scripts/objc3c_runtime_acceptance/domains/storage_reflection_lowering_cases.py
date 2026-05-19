"""Storage/reflection lowering acceptance case surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_cases import (
    check_property_accessor_layout_lowering_case,
    check_synthesized_accessor_codegen_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_cases import (
    check_accessor_storage_lowering_metadata_surface_case,
)

_EXPORTED_CASE_NAMES = [
    "check_accessor_storage_lowering_metadata_surface_case",
    "check_property_accessor_layout_lowering_case",
    "check_synthesized_accessor_codegen_case",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
