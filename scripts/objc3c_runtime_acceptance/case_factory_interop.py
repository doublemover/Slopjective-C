"""Interop packaging case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories


def build_interop_packaging_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "cross-module-runtime-package-interop-source-surface",
            lambda: domains.interop_packaging.check_cross_module_runtime_package_interop_source_surface_case(
                run_dir
            ),
        ),
        (
            "textual-binary-interface-parity-source-surface",
            lambda: domains.interop_packaging.check_textual_binary_interface_parity_source_surface_case(
                run_dir
            ),
        ),
        (
            "mixed-image-interop-semantics",
            lambda: domains.interop_packaging.check_mixed_image_interop_semantics_case(
                run_dir
            ),
        ),
        (
            "c-cpp-swift-interop-boundary-semantics",
            lambda: domains.interop_packaging.check_c_cpp_swift_bridge_semantics_case(
                run_dir
            ),
        ),
        (
            "import-version-feature-claim-diagnostics",
            lambda: domains.interop_packaging.check_import_version_feature_claim_diagnostics_case(
                run_dir
            ),
        ),
        (
            "runtime-packaging-bridge-loader-artifact-surface",
            lambda: domains.interop_packaging.check_runtime_packaging_bridge_loader_artifact_surface_case(
                run_dir
            ),
        ),
        (
            "mixed-image-package-lowering-bridge-emission",
            lambda: domains.interop_packaging.check_mixed_image_package_lowering_bridge_emission_case(
                run_dir
            ),
        ),
        (
            "cross-language-replay-import-surface-preservation",
            lambda: domains.interop_packaging.check_cross_language_replay_import_surface_preservation_case(
                run_dir
            ),
        ),
        (
            "runtime-package-loader-bridge-abi",
            lambda: domains.interop_packaging.check_runtime_package_loader_bridge_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-package-loading-interop-runtime-implementation",
            lambda: domains.interop_packaging.check_live_package_loading_interop_runtime_implementation_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "imported-runtime-packaging-replay",
            lambda: domains.interop_packaging.check_imported_runtime_packaging_replay_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


__all__ = ["build_interop_packaging_case_factories"]
