"""Release-claims publication runtime acceptance surfaces."""

from __future__ import annotations

from typing import Any

from ..case_result import CaseResult
from ..core import (
    DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
)


def build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {"scaffold-retirement-deprecated-sidecar-compatibility-diagnostics"}
    ]
    return {
        "contract_id": (
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
            "objc3c.driver.conformance.report.publication.v1",
            "objc3c.toolchain.conformance.claim.operations.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp",
        ],
        "compatibility_diagnostic_model": (
            "live-compile-and-validation-fail-closed-when-deprecated-claim-or-scaffold-sidecars-appear-next-to-current-release-artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
        "explicit_non_goals": [
            "no-silent-compatibility-with-retired-sidecars",
            "no-separate-migration-path-for-deprecated-claim-artifacts",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_claim_publication_dashboard_schema_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"claim-publication-dashboard-schema-surface"}
    ]
    return {
        "contract_id": RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
            "objc3c.toolchain.dashboard.status.publication.v1",
            "objc3c.toolchain.release.evidence.toolchain.operations.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
            "<emit-prefix>.objc3-release-evidence-operation.json",
            "<emit-prefix>.objc3-dashboard-status.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        ],
        "dashboard_schema_model": (
            "validation-publishes-a-schema-shaped-claim-dashboard-over-the-live-report-publication-validation-and-release-evidence-artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "dashboard_artifact_path": "<emit-prefix>.objc3-dashboard-status.json",
        "dashboard_schema_path": "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "explicit_non_goals": [
            "no-dashboard-built-from-release-scope-notes",
            "no-schema-drift-hidden-behind-ad-hoc-dashboard-fields",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


def build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"final-claim-publication-deprecated-path-shutdown"}
    ]
    return {
        "contract_id": (
            RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
            "objc3c.tooling.integrated.advanced.feature.gate.v1",
            "objc3c.tooling.release.candidate.execution.matrix.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.objc3-conformance-report.json",
            "<emit-prefix>.objc3-conformance-publication.json",
            "<emit-prefix>.objc3-conformance-validation.json",
            "<emit-prefix>.objc3-release-evidence-operation.json",
            "<emit-prefix>.objc3-dashboard-status.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
        ],
        "final_publication_model": (
            "compile-publishes-the-live-claim-report-publication-and-initial-release-artifacts-validation-completes-the-final-claim-publication-bundle-and-no-deprecated-sidecars-remain"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
        "explicit_non_goals": [
            "no-dashboard-ready-summary-revival",
            "no-toolchain-runtime-ga-scaffold-output-revival",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


__all__ = [
    "build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
    "build_runtime_claim_publication_dashboard_schema_surface",
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface",
]
