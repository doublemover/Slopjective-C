"""Case command construction for the mixed-module differential runner."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import cast

from .models import CaseResult, ClangCase, NoClangCase, SurfaceBuilder
from .runtime_acceptance import interop_packaging


def run_no_clang(fn: NoClangCase, run_dir: Path, _: str | None) -> CaseResult:
    return fn(run_dir)


def run_with_clang(fn: ClangCase, run_dir: Path, clangxx: str | None) -> CaseResult:
    if not clangxx:
        raise RuntimeError("mixed-module differential requires clang++ for the selected case set")
    return fn(clangxx, run_dir)


@dataclass(frozen=True)
class CaseCommand:
    case_id: str
    runner: NoClangCase | ClangCase
    requires_clangxx: bool = False
    result_case_id: str | None = None

    def __call__(self, run_dir: Path, clangxx: str | None) -> CaseResult:
        if self.requires_clangxx:
            return run_with_clang(cast(ClangCase, self.runner), run_dir, clangxx)
        return run_no_clang(cast(NoClangCase, self.runner), run_dir, clangxx)

    @property
    def expected_result_case_id(self) -> str:
        return self.result_case_id or self.case_id


CASE_RUNNERS: dict[str, CaseCommand] = {
    "cross-module-runtime-package-interop-source-surface": CaseCommand(
        "cross-module-runtime-package-interop-source-surface",
        interop_packaging.check_cross_module_runtime_package_interop_source_surface_case,
    ),
    "textual-binary-interface-parity-source-surface": CaseCommand(
        "textual-binary-interface-parity-source-surface",
        interop_packaging.check_textual_binary_interface_parity_source_surface_case,
    ),
    "mixed-image-compatibility-interop-semantics": CaseCommand(
        "mixed-image-compatibility-interop-semantics",
        interop_packaging.check_mixed_image_compatibility_interop_semantics_case,
    ),
    "imported-runtime-packaging-replay": CaseCommand(
        "imported-runtime-packaging-replay",
        interop_packaging.check_imported_runtime_packaging_replay_case,
        requires_clangxx=True,
    ),
    "c-cpp-swift-interop-boundary-semantics": CaseCommand(
        "c-cpp-swift-interop-boundary-semantics",
        interop_packaging.check_c_cpp_swift_bridge_compatibility_semantics_case,
    ),
    "import-version-feature-claim-diagnostics": CaseCommand(
        "import-version-feature-claim-diagnostics",
        interop_packaging.check_import_version_feature_claim_diagnostics_case,
    ),
    "runtime-packaging-bridge-loader-artifact-surface": CaseCommand(
        "runtime-packaging-bridge-loader-artifact-surface",
        interop_packaging.check_runtime_packaging_bridge_loader_artifact_surface_case,
    ),
    "mixed-image-package-lowering-bridge-emission": CaseCommand(
        "mixed-image-package-lowering-bridge-emission",
        interop_packaging.check_mixed_image_package_lowering_bridge_emission_case,
    ),
    "cross-language-replay-import-surface-preservation": CaseCommand(
        "cross-language-replay-import-surface-preservation",
        interop_packaging.check_cross_language_replay_import_surface_preservation_case,
    ),
    "runtime-package-loader-bridge-abi": CaseCommand(
        "runtime-package-loader-bridge-abi",
        interop_packaging.check_runtime_package_loader_bridge_abi_case,
        requires_clangxx=True,
    ),
    "live-package-loading-interop-runtime-implementation": CaseCommand(
        "live-package-loading-interop-runtime-implementation",
        interop_packaging.check_live_package_loading_interop_runtime_implementation_case,
        requires_clangxx=True,
    ),
}

SURFACE_BUILDERS: dict[str, SurfaceBuilder] = {
    "runtime_cross_module_package_interop_source_surface": (
        interop_packaging.build_runtime_cross_module_package_interop_source_surface
    ),
    "runtime_textual_binary_interface_parity_source_surface": (
        interop_packaging.build_runtime_textual_binary_interface_parity_source_surface
    ),
    "runtime_mixed_image_compatibility_interop_semantics_surface": (
        interop_packaging.build_runtime_mixed_image_compatibility_interop_semantics_surface
    ),
    "runtime_package_loading_module_identity_semantics_surface": (
        interop_packaging.build_runtime_package_loading_module_identity_semantics_surface
    ),
    "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": (
        interop_packaging.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface
    ),
    "runtime_import_version_feature_claim_diagnostics_surface": (
        interop_packaging.build_runtime_import_version_feature_claim_diagnostics_surface
    ),
    "runtime_packaging_bridge_loader_artifact_surface": (
        interop_packaging.build_runtime_packaging_bridge_loader_artifact_surface
    ),
    "runtime_mixed_image_package_lowering_bridge_emission_surface": (
        interop_packaging.build_runtime_mixed_image_package_lowering_bridge_emission_surface
    ),
    "runtime_cross_language_replay_import_surface_preservation_surface": (
        interop_packaging.build_runtime_cross_language_replay_import_surface_preservation_surface
    ),
    "runtime_package_loader_bridge_abi_surface": interop_packaging.build_runtime_package_loader_bridge_abi_surface,
    "runtime_package_loading_interop_implementation_surface": (
        interop_packaging.build_runtime_package_loading_interop_implementation_surface
    ),
}

__all__ = [
    "CASE_RUNNERS",
    "SURFACE_BUILDERS",
    "CaseCommand",
    "run_no_clang",
    "run_with_clang",
]
