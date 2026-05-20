"""Interop and packaging runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.package.interop.source.surface.v1"
)
RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.textual.binary.interface.parity.source.surface.v1"
)
RUNTIME_MIXED_IMAGE_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.mixed.image.interop.semantics.surface.v1"
)
RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loading.module.identity.semantics.surface.v1"
)
RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.c.cpp.swift.bridge.semantics.surface.v1"
)
RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.import.version.feature.claim.diagnostics.surface.v1"
)
RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.packaging.bridge.loader.artifact.surface.v1"
)
RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.mixed.image.package.lowering.bridge.emission.surface.v1"
)
RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.language.replay.import.surface.preservation.surface.v1"
)
RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loader.bridge.abi.surface.v1"
)
RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.package.loading.interop.implementation.surface.v1"
)

IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_provider.objc3"
)
IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)
IMPORTED_RUNTIME_PACKAGING_PROBE = (
    "tests/tooling/runtime/import_module_execution_matrix_probe.cpp"
)
INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3"
)
INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3"
)
INTEROP_HEADER_MODULE_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/header_module_bridge_provider.objc3"
)
INTEROP_HEADER_MODULE_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
)
INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp"
)
INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/header_module_bridge_generation_probe.cpp"
)
INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE = (
    "tests/tooling/runtime/package_loader_fail_closed_diagnostics_probe.cpp"
)


__all__ = [name for name in globals() if name.isupper()]
