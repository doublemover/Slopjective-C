"""Interop packaging semantic contract surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_mixed_image_compatibility_interop_semantics_surface",
    "build_runtime_package_loading_module_identity_semantics_surface",
    "build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface",
]


def build_runtime_mixed_image_compatibility_interop_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"mixed-image-compatibility-interop-semantics"}
    ]
    return {
        "contract_id": (
            RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "language_profile_model": (
            "mixed-image-provider-and-consumer-compiles-share-one-fail-closed-registration-order-and-interop-bridge-compatibility-boundary-through-runtime-import-surfaces-and-cross-module-link-plans"
        ),
        "diagnostic_model": (
            "duplicate-registration-order-or-import-surface-drift-rejects-the-consumer-before-cross-module-link-plan-installation-advances"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
            INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_package_loading_module_identity_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "multi-image-registration-reset-replay",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.obj",
        ],
        "runtime_probe": IMPORTED_RUNTIME_PACKAGING_PROBE,
        "package_loading_model": (
            "runtime-package-loading-and-reset-replay-preserve-imported-and-local-module-identities-registration-ordinals-and-realized-class-ownership-through-the-live-runtime"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }


def build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"c-cpp-swift-bridge-compatibility-semantics"}
    ]
    return {
        "contract_id": (
            RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "language_profile_model": (
            "c-cpp-and-swift-facing-interop-annotations-survive-provider-emission-consumer-import-and-cross-module-link-planning-without-bridge-shape-drift"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
