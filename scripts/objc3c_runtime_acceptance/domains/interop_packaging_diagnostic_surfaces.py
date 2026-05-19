"""Interop packaging diagnostic contract surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..runtime_contract_interop import (
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_import_version_feature_claim_diagnostics_surface",
]


def build_runtime_import_version_feature_claim_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"import-version-feature-claim-diagnostics"}
    ]
    return {
        "contract_id": (
            RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
            "<emit-prefix>.diagnostics.json",
        ],
        "diagnostic_model": (
            "shipped-feature-claim-sidecars-stay-ready-while-drifted-import-contract-versions-fail-closed-before-consumer-packaging-claims-advance"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_release_artifact_sidecars": True,
        "requires_real_compile_output": True,
    }


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
