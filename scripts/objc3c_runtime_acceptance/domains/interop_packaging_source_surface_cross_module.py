"""Cross-module interop packaging source contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_cross_module_package_interop_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-module-runtime-package-interop-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "source_contract_ids": [
            "objc3c.interop.foreign.declaration.import.source.closure.v1",
            "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/pipeline/objc3_frontend_types.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_interop_foreign_declaration_and_import_source_closure",
            "frontend.pipeline.semantic_surface.objc_interop_cpp_and_swift_interop_annotation_source_completion",
            "frontend.pipeline.semantic_surface.objc_interop_foreign_surface_interface_and_module_preservation",
            "frontend.pipeline.semantic_surface.objc_interop_header_module_and_bridge_generation",
        ],
        "source_surface_model": (
            "cross-module-runtime-packaging-publishes-one-compile-coupled-import-surface-and-bridge-artifact-boundary-for-foreign-cpp-and-swift-facing-interop-facts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        ],
        "explicit_non_goals": [
            "no-live-foreign-symbol-linking-claim",
            "no-cross-language-runnable-execution-claim",
            "no-public-runtime-abi-widening",
        ],
        "requires_runtime_import_surface": True,
        "requires_real_compile_output": True,
    }


__all__ = ["build_runtime_cross_module_package_interop_source_surface"]
