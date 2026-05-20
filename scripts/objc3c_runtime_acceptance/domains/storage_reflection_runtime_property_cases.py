"""Storage/reflection property runtime acceptance case surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_cases import (
    check_storage_ownership_reflection_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_property_execution_cases import (
    check_property_execution_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_property_invalid_layout_cases import (
    check_property_invalid_layout_runtime_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_property_reflection_cases import (
    check_property_reflection_case,
)

_EXPORTED_CASE_NAMES = [
    "check_property_reflection_case",
    "check_property_invalid_layout_runtime_case",
    "check_property_execution_case",
    "check_storage_ownership_reflection_case",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
