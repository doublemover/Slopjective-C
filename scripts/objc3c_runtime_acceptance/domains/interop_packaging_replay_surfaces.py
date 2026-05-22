"""Interop packaging replay contract surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_cross_language_replay_import_surface_preservation_surface",
]


def build_runtime_cross_language_replay_import_surface_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-language-replay-import-surface-preservation"}
    ]
    return {
        "contract_id": (
            RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_C_CPP_SWIFT_BRIDGE_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
        ],
        "preservation_model": (
            "provider-runtime-import-replay-keys-for-c-cpp-and-swift-interop-survive-in-the-deferred-import-surface-while-consumer-cross-module-link-planning-fails-closed-until-live-bridge-generation-is-active"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
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
