"""Interop packaging lowering contract surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_mixed_image_package_lowering_bridge_emission_surface",
]


def build_runtime_mixed_image_package_lowering_bridge_emission_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"mixed-image-package-lowering-bridge-emission"}
    ]
    return {
        "contract_id": (
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.ll",
            "<emit-prefix>.runtime-import-surface.json",
        ],
        "deferred_bridge_artifact_paths": [
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "lowering_model": (
            "provider-lowering-emits-interop-abi-summary-metadata-and-bridge-call-declarations-while-runtime-import-surfaces-preserve-deferred-bridge-paths-without-claiming-consumer-cross-module-link-plan-readiness"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/lower/objc3_lowering_contract.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": False,
        "requires_fail_closed_consumer_import": True,
        "requires_real_compile_output": True,
    }


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
