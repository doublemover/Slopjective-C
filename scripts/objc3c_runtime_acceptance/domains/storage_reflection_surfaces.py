"""Storage/reflection runtime acceptance surface builder surface."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_surfaces import (
    build_dispatch_and_synthesized_accessor_lowering_surface,
    build_executable_ivar_layout_emission_surface,
    build_executable_property_accessor_layout_lowering_surface,
    build_executable_synthesized_accessor_property_lowering_surface,
)
from objc3c_runtime_acceptance.domains.storage_reflection_source_surfaces import (
    build_runtime_property_atomicity_synthesis_reflection_source_surface,
    build_runtime_property_ivar_storage_accessor_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_property_ivar_storage_accessor_source_surface",
    "build_runtime_property_atomicity_synthesis_reflection_source_surface",
    "build_dispatch_and_synthesized_accessor_lowering_surface",
    "build_executable_property_accessor_layout_lowering_surface",
    "build_executable_ivar_layout_emission_surface",
    "build_executable_synthesized_accessor_property_lowering_surface",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
