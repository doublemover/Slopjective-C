"""Static contract and report paths for runnable interop conformance."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-interop-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.interop.conformance.summary.v1"

REQUIRED_CASES = {
    "cross-module-runtime-package-interop-source-surface",
    "textual-binary-interface-parity-source-surface",
    "mixed-image-interop-semantics",
    "imported-runtime-packaging-replay",
    "c-cpp-swift-interop-boundary-semantics",
    "import-version-feature-claim-diagnostics",
    "runtime-packaging-bridge-loader-artifact-surface",
    "mixed-image-package-lowering-bridge-emission",
    "cross-language-replay-import-surface-preservation",
    "runtime-package-loader-bridge-abi",
    "live-package-loading-interop-runtime-implementation",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_cross_module_package_interop_source_surface": (
        "objc3c.runtime.cross.module.package.interop.source.surface.v1"
    ),
    "runtime_textual_binary_interface_parity_source_surface": (
        "objc3c.runtime.textual.binary.interface.parity.source.surface.v1"
    ),
    "runtime_mixed_image_interop_semantics_surface": (
        "objc3c.runtime.mixed.image.interop.semantics.surface.v1"
    ),
    "runtime_package_loading_module_identity_semantics_surface": (
        "objc3c.runtime.package.loading.module.identity.semantics.surface.v1"
    ),
    "runtime_c_cpp_swift_bridge_semantics_surface": (
        "objc3c.runtime.c.cpp.swift.bridge.semantics.surface.v1"
    ),
    "runtime_import_version_feature_claim_diagnostics_surface": (
        "objc3c.runtime.import.version.feature.claim.diagnostics.surface.v1"
    ),
    "runtime_packaging_bridge_loader_artifact_surface": (
        "objc3c.runtime.packaging.bridge.loader.artifact.surface.v1"
    ),
    "runtime_mixed_image_package_lowering_bridge_emission_surface": (
        "objc3c.runtime.mixed.image.package.lowering.bridge.emission.surface.v1"
    ),
    "runtime_cross_language_replay_import_surface_preservation_surface": (
        "objc3c.runtime.cross.language.replay.import.surface.preservation.surface.v1"
    ),
    "runtime_package_loader_bridge_abi_surface": (
        "objc3c.runtime.package.loader.bridge.abi.surface.v1"
    ),
    "runtime_package_loading_interop_implementation_surface": (
        "objc3c.runtime.package.loading.interop.implementation.surface.v1"
    ),
}


__all__ = [
    "ACCEPTANCE_REPORT",
    "INTEGRATION_REPORT",
    "REPORT_PATH",
    "REQUIRED_CASES",
    "REQUIRED_SURFACE_CONTRACTS",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
]
