"""Storage/reflection runtime acceptance case surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_runtime_accessor_cases import (
    check_synthesized_accessor_runtime_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_layout_cases import (
    check_instance_allocation_layout_runtime_case,
    check_property_layout_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_property_cases import (
    check_property_execution_case,
    check_property_invalid_layout_runtime_case,
    check_property_reflection_case,
    check_storage_ownership_reflection_case,
)

_EXPORTED_CASE_NAMES = [
    "check_property_reflection_case",
    "check_property_invalid_layout_runtime_case",
    "check_property_execution_case",
    "check_storage_ownership_reflection_case",
    "check_synthesized_accessor_runtime_case",
    "check_property_layout_case",
    "check_instance_allocation_layout_runtime_case",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
