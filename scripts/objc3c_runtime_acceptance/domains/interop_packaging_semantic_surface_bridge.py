"""C/C++/Swift interop boundary semantic contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"c-cpp-swift-interop-boundary-semantics"}
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
            "c-cpp-and-swift-facing-interop-annotations-survive-provider-emission-consumer-import-and-cross-module-link-planning-without-interop-boundary-shape-drift"
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


__all__ = ["build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface"]
