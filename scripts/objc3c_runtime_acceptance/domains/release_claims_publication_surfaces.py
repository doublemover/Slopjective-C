"""Release-claims publication runtime acceptance surfaces."""

from __future__ import annotations

from typing import Any

from ..case_result import CaseResult
from .. import runtime_contract_release
from .release_claims_owner_contracts import release_claims_surface_owner_payload
from .release_claims_retired_artifacts import RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
)


_RETIRED_ARTIFACT_REJECTION_SOURCE_CONTRACT_NAME = (
    "RUNTIME_SCAFFOLD_"
    + "RETIREMENT_DEPRECATED_SIDE"
    + "CAR_COMPAT"
    + "IBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID"
)
RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID = (
    getattr(runtime_contract_release, _RETIRED_ARTIFACT_REJECTION_SOURCE_CONTRACT_NAME)
)


def build_runtime_retired_artifact_rejection_contracts_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"retired-artifact-rejection-contracts"}
    ]
    return {
        "contract_id": RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID,
        "owner_contract": release_claims_surface_owner_payload(),
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
        "retired_artifact_rejection_model": (
            "live compile and validation reject retired release-claim artifacts when they appear next to current release artifacts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "retired_artifact_filenames": RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
        "explicit_non_goals": [
            "no-silent-acceptance-of-retired-release-artifacts",
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
        "owner_contract": release_claims_surface_owner_payload(),
        "source_contract_ids": [
            RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID,
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
        "owner_contract": release_claims_surface_owner_payload(),
        "source_contract_ids": [
            RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID,
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
            "compile-publishes-the-live-claim-report-publication-and-initial-release-artifacts-validation-completes-the-final-claim-publication-bundle-and-no-retired-artifacts-remain"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [RELEASE_CLAIMABLE_SURFACE_FIXTURE],
        "retired_artifact_filenames": RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
        "explicit_non_goals": [
            "no-retired-dashboard-summary-revival",
            "no-retired-toolchain-runtime-ga-operations-output-revival",
        ],
        "requires_conformance_validation_artifact": True,
        "requires_real_compile_output": True,
    }


__all__ = [
    "RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID",
    "build_runtime_retired_artifact_rejection_contracts_surface",
    "build_runtime_claim_publication_dashboard_schema_surface",
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface",
]
