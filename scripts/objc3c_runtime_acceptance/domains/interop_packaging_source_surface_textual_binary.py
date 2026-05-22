"""Textual/binary interop packaging source contract surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_textual_binary_interface_parity_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"textual-binary-interface-parity-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-import-surface.json",
        ],
        "deferred_bridge_artifact_paths": [
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "source_contract_ids": [
            "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/pipeline/objc3_frontend_types.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_interop_cpp_and_swift_interop_annotation_source_completion",
            "frontend.pipeline.semantic_surface.objc_interop_foreign_surface_interface_and_module_preservation",
            "frontend.pipeline.semantic_surface.objc_interop_header_module_and_bridge_generation",
        ],
        "parity_surface_model": (
            "runtime-import-surface-and-source-manifest-preserve-one-foreign-cpp-and-swift-facing-interface-shape-plus-deferred-bridge-artifact-paths-without-claiming-generated-header-modulemap-or-bridge-json-artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        ],
        "requires_runtime_import_surface": True,
        "requires_textual_bridge_artifacts": False,
        "requires_deferred_bridge_paths": True,
        "requires_real_compile_output": True,
    }


__all__ = ["build_runtime_textual_binary_interface_parity_source_surface"]
