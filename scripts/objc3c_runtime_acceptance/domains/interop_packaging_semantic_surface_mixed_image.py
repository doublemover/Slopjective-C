"""Mixed-image interop packaging semantic contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_mixed_image_interop_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"mixed-image-interop-semantics"}
    ]
    return {
        "contract_id": (
            RUNTIME_MIXED_IMAGE_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "deferred_bridge_artifact_paths": [
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "language_profile_model": (
            "mixed-image-provider-and-consumer-compiles-share-one-fail-closed-registration-order-and-deferred-interop-boundary-through-runtime-import-surfaces-without claiming generated bridge artifacts or installing a ready cross-module link plan"
        ),
        "diagnostic_model": (
            "duplicate-registration-order-or-incomplete-interop-import-surface-rejects-the-consumer-before-cross-module-link-plan-installation-advances"
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
        "requires_cross_module_link_plan_artifact": False,
        "requires_fail_closed_consumer_import": True,
        "requires_real_compile_output": True,
    }


__all__ = ["build_runtime_mixed_image_interop_semantics_surface"]
