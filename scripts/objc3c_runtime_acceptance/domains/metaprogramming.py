"""Metaprogramming runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.metaprogramming_cross_module_cases import (
    check_cross_module_metaprogramming_artifact_preservation_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_derive_property_cases import (
    check_metaprogramming_derive_property_behavior_semantics_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_executable_lowering_cases import (
    check_metaprogramming_executable_lowering_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_lowering_host_cache_cases import (
    check_metaprogramming_lowering_host_cache_surface_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_macro_safety_cases import (
    check_metaprogramming_macro_safety_cache_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_cases import (
    check_live_metaprogramming_cache_runtime_integration_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_helpers import (
    remove_metaprogramming_cache_entry_from_artifact,
)
from objc3c_runtime_acceptance.domains.metaprogramming_runtime_abi_cases import (
    check_metaprogramming_runtime_abi_cache_surface_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_semantic_model_cases import (
    check_metaprogramming_semantics_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_source_cases import (
    check_metaprogramming_package_provenance_source_surface_case,
    check_metaprogramming_source_surface_case,
)
from objc3c_runtime_acceptance.domains.metaprogramming_lowering_surfaces import (
    RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
    build_runtime_cross_module_metaprogramming_artifact_preservation_surface,
    build_runtime_metaprogramming_lowering_host_cache_surface,
)
from objc3c_runtime_acceptance.domains.metaprogramming_runtime_cache_surfaces import (
    METAPROGRAMMING_EXPANSION_RUNTIME_MODEL,
    METAPROGRAMMING_HOST_CACHE_RUNTIME_MODEL,
    METAPROGRAMMING_RUNTIME_ABI_BOUNDARY_MODEL,
    METAPROGRAMMING_RUNTIME_FAIL_CLOSED_MODEL,
    PRIVATE_METAPROGRAMMING_RUNTIME_ABI_BOUNDARY,
    RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
    build_runtime_metaprogramming_cache_runtime_integration_implementation_surface,
    build_runtime_metaprogramming_runtime_abi_cache_surface,
)
from objc3c_runtime_acceptance.domains.metaprogramming_semantic_surfaces import (
    RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
    build_runtime_metaprogramming_semantics_surface,
)
from objc3c_runtime_acceptance.domains.metaprogramming_source_surfaces import (
    RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
    build_runtime_metaprogramming_package_provenance_source_surface,
    build_runtime_metaprogramming_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_metaprogramming_source_surface",
    "build_runtime_metaprogramming_package_provenance_source_surface",
    "build_runtime_metaprogramming_semantics_surface",
    "build_runtime_metaprogramming_lowering_host_cache_surface",
    "build_runtime_cross_module_metaprogramming_artifact_preservation_surface",
    "build_runtime_metaprogramming_runtime_abi_cache_surface",
    "build_runtime_metaprogramming_cache_runtime_integration_implementation_surface",
    "check_metaprogramming_source_surface_case",
    "check_metaprogramming_package_provenance_source_surface_case",
    "check_metaprogramming_semantics_case",
    "check_metaprogramming_derive_property_behavior_semantics_case",
    "check_metaprogramming_macro_safety_cache_diagnostics_case",
    "check_metaprogramming_lowering_host_cache_surface_case",
    "check_metaprogramming_executable_lowering_case",
    "check_cross_module_metaprogramming_artifact_preservation_case",
    "check_metaprogramming_runtime_abi_cache_surface_case",
    "check_live_metaprogramming_cache_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
