"""Object Model query and lookup acceptance surface builder exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.object_model_surface_query_abi import (
    build_runtime_object_model_abi_query_surface,
)
from objc3c_runtime_acceptance.domains.object_model_surface_query_implementation import (
    build_runtime_realization_lookup_reflection_implementation_surface,
)
from objc3c_runtime_acceptance.domains.object_model_surface_query_reflection import (
    build_runtime_reflection_query_surface,
)
from objc3c_runtime_acceptance.domains.object_model_surface_query_semantics import (
    build_runtime_realization_lookup_semantics_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_object_model_abi_query_surface",
    "build_runtime_realization_lookup_reflection_implementation_surface",
    "build_runtime_reflection_query_surface",
    "build_runtime_realization_lookup_semantics_surface",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
