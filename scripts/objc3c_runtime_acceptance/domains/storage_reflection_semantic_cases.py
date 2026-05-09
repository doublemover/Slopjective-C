"""Storage/reflection semantic acceptance case surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_semantic_diagnostic_cases import (
    check_property_reflection_accessor_compatibility_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_semantic_legality_cases import (
    check_storage_legality_semantics_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_semantic_ordering_cases import (
    check_property_ivar_ordering_semantics_case,
)
from objc3c_runtime_acceptance.domains.storage_reflection_semantic_synthesis_cases import (
    check_property_synthesis_storage_binding_semantics_case,
)

_EXPORTED_CASE_NAMES = [
    "check_storage_legality_semantics_case",
    "check_property_synthesis_storage_binding_semantics_case",
    "check_property_reflection_accessor_compatibility_diagnostics_case",
    "check_property_ivar_ordering_semantics_case",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
