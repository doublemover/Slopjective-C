"""Interop Packaging runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_artifact_cases import (
    check_runtime_packaging_bridge_loader_artifact_surface_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_artifact_surfaces import (
    build_runtime_packaging_bridge_loader_artifact_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_diagnostic_cases import (
    check_import_version_feature_claim_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_diagnostic_surfaces import (
    build_runtime_import_version_feature_claim_diagnostics_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_cases import (
    check_imported_runtime_packaging_replay_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_lowering_cases import (
    check_mixed_image_package_lowering_bridge_emission_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_lowering_surfaces import (
    build_runtime_mixed_image_package_lowering_bridge_emission_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_replay_cases import (
    check_cross_language_replay_import_surface_preservation_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_replay_surfaces import (
    build_runtime_cross_language_replay_import_surface_preservation_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_runtime_cases import (
    check_live_package_loading_interop_runtime_implementation_case,
    check_runtime_package_loader_bridge_abi_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_runtime_surfaces import (
    build_runtime_package_loader_bridge_abi_surface,
    build_runtime_package_loading_interop_implementation_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_cases import (
    check_c_cpp_swift_bridge_semantics_case,
    check_mixed_image_interop_semantics_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_surfaces import (
    build_runtime_c_cpp_swift_bridge_semantics_surface,
    build_runtime_mixed_image_interop_semantics_surface,
    build_runtime_package_loading_module_identity_semantics_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_source_cases import (
    check_cross_module_runtime_package_interop_source_surface_case,
    check_textual_binary_interface_parity_source_surface_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_source_surfaces import (
    build_runtime_cross_module_package_interop_source_surface,
    build_runtime_textual_binary_interface_parity_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_cross_module_package_interop_source_surface",
    "build_runtime_textual_binary_interface_parity_source_surface",
    "build_runtime_mixed_image_interop_semantics_surface",
    "build_runtime_package_loading_module_identity_semantics_surface",
    "build_runtime_c_cpp_swift_bridge_semantics_surface",
    "build_runtime_import_version_feature_claim_diagnostics_surface",
    "build_runtime_packaging_bridge_loader_artifact_surface",
    "build_runtime_mixed_image_package_lowering_bridge_emission_surface",
    "build_runtime_cross_language_replay_import_surface_preservation_surface",
    "build_runtime_package_loader_bridge_abi_surface",
    "build_runtime_package_loading_interop_implementation_surface",
    "check_imported_runtime_packaging_replay_case",
    "check_cross_module_runtime_package_interop_source_surface_case",
    "check_textual_binary_interface_parity_source_surface_case",
    "check_mixed_image_interop_semantics_case",
    "check_c_cpp_swift_bridge_semantics_case",
    "check_import_version_feature_claim_diagnostics_case",
    "check_runtime_packaging_bridge_loader_artifact_surface_case",
    "check_mixed_image_package_lowering_bridge_emission_case",
    "check_cross_language_replay_import_surface_preservation_case",
    "check_runtime_package_loader_bridge_abi_case",
    "check_live_package_loading_interop_runtime_implementation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
