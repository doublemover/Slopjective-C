"""Storage/reflection lowering layout assertion facade."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_codegen_assertions import (
    assert_registration_manifest_layout_counts,
    assert_synthesized_accessor_codegen_manifest,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_contract_assertions import (
    ManifestSurface,
    assert_accessor_layout_surface,
    assert_executable_synthesized_accessor_surface,
    assert_ivar_layout_surface,
    assert_property_source_surface_links,
)


__all__ = [
    "ManifestSurface",
    "assert_accessor_layout_surface",
    "assert_executable_synthesized_accessor_surface",
    "assert_ivar_layout_surface",
    "assert_property_source_surface_links",
    "assert_registration_manifest_layout_counts",
    "assert_synthesized_accessor_codegen_manifest",
]
