"""Interop and packaging summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult


def build_interop_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_cross_module_package_interop_source_surface": (
            domains.interop_packaging.build_runtime_cross_module_package_interop_source_surface(
                results
            )
        ),
        "runtime_textual_binary_interface_parity_source_surface": (
            domains.interop_packaging.build_runtime_textual_binary_interface_parity_source_surface(
                results
            )
        ),
        "runtime_mixed_image_compatibility_interop_semantics_surface": (
            domains.interop_packaging.build_runtime_mixed_image_compatibility_interop_semantics_surface(
                results
            )
        ),
        "runtime_package_loading_module_identity_semantics_surface": (
            domains.interop_packaging.build_runtime_package_loading_module_identity_semantics_surface(
                results
            )
        ),
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": (
            domains.interop_packaging.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
                results
            )
        ),
        "runtime_import_version_feature_claim_diagnostics_surface": (
            domains.interop_packaging.build_runtime_import_version_feature_claim_diagnostics_surface(
                results
            )
        ),
        "runtime_packaging_bridge_loader_artifact_surface": (
            domains.interop_packaging.build_runtime_packaging_bridge_loader_artifact_surface(
                results
            )
        ),
        "runtime_mixed_image_package_lowering_bridge_emission_surface": (
            domains.interop_packaging.build_runtime_mixed_image_package_lowering_bridge_emission_surface(
                results
            )
        ),
        "runtime_cross_language_replay_import_surface_preservation_surface": (
            domains.interop_packaging.build_runtime_cross_language_replay_import_surface_preservation_surface(
                results
            )
        ),
        "runtime_package_loader_bridge_abi_surface": (
            domains.interop_packaging.build_runtime_package_loader_bridge_abi_surface(
                results
            )
        ),
        "runtime_package_loading_interop_implementation_surface": (
            domains.interop_packaging.build_runtime_package_loading_interop_implementation_surface(
                results
            )
        ),
    }


__all__ = ["build_interop_summary_sections"]
