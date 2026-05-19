"""Live targeted interop cases and surface builders."""

from __future__ import annotations

import tempfile
from pathlib import Path
from typing import Any

import check_objc3c_runtime_acceptance as runtime_acceptance
from objc3c_tooling.paths import repo_rel

from .constants import ROOT


def collect_live_results() -> tuple[list[runtime_acceptance.CaseResult], str]:
    live_case_root = ROOT / "tmp" / "reports" / "runtime" / "runnable-interop-conformance" / "live-case"
    live_case_root.mkdir(parents=True, exist_ok=True)
    clangxx = runtime_acceptance.find_clangxx()
    with tempfile.TemporaryDirectory(dir=live_case_root) as tmp_dir:
        run_dir = Path(tmp_dir)
        results = [
            runtime_acceptance.check_cross_module_runtime_package_interop_source_surface_case(run_dir),
            runtime_acceptance.check_textual_binary_interface_parity_source_surface_case(run_dir),
            runtime_acceptance.check_mixed_image_interop_semantics_case(run_dir),
            runtime_acceptance.check_imported_runtime_packaging_replay_case(clangxx, run_dir),
            runtime_acceptance.check_c_cpp_swift_bridge_semantics_case(run_dir),
            runtime_acceptance.check_import_version_feature_claim_diagnostics_case(run_dir),
            runtime_acceptance.check_runtime_packaging_bridge_loader_artifact_surface_case(run_dir),
            runtime_acceptance.check_mixed_image_package_lowering_bridge_emission_case(run_dir),
            runtime_acceptance.check_cross_language_replay_import_surface_preservation_case(run_dir),
            runtime_acceptance.check_runtime_package_loader_bridge_abi_case(clangxx, run_dir),
            runtime_acceptance.check_live_package_loading_interop_runtime_implementation_case(clangxx, run_dir),
        ]
        run_dir_rel = repo_rel(run_dir)
    return results, run_dir_rel


def build_live_surfaces(results: list[runtime_acceptance.CaseResult]) -> dict[str, dict[str, Any]]:
    return {
        "runtime_cross_module_package_interop_source_surface": runtime_acceptance.build_runtime_cross_module_package_interop_source_surface(
            results
        ),
        "runtime_textual_binary_interface_parity_source_surface": runtime_acceptance.build_runtime_textual_binary_interface_parity_source_surface(
            results
        ),
        "runtime_mixed_image_interop_semantics_surface": runtime_acceptance.build_runtime_mixed_image_interop_semantics_surface(
            results
        ),
        "runtime_package_loading_module_identity_semantics_surface": runtime_acceptance.build_runtime_package_loading_module_identity_semantics_surface(
            results
        ),
        "runtime_c_cpp_swift_bridge_semantics_surface": runtime_acceptance.build_runtime_c_cpp_swift_bridge_semantics_surface(
            results
        ),
        "runtime_import_version_feature_claim_diagnostics_surface": runtime_acceptance.build_runtime_import_version_feature_claim_diagnostics_surface(
            results
        ),
        "runtime_packaging_bridge_loader_artifact_surface": runtime_acceptance.build_runtime_packaging_bridge_loader_artifact_surface(
            results
        ),
        "runtime_mixed_image_package_lowering_bridge_emission_surface": runtime_acceptance.build_runtime_mixed_image_package_lowering_bridge_emission_surface(
            results
        ),
        "runtime_cross_language_replay_import_surface_preservation_surface": runtime_acceptance.build_runtime_cross_language_replay_import_surface_preservation_surface(
            results
        ),
        "runtime_package_loader_bridge_abi_surface": runtime_acceptance.build_runtime_package_loader_bridge_abi_surface(
            results
        ),
        "runtime_package_loading_interop_implementation_surface": runtime_acceptance.build_runtime_package_loading_interop_implementation_surface(
            results
        ),
    }


__all__ = ["build_live_surfaces", "collect_live_results", "runtime_acceptance"]
