"""Release-claims runtime ABI and evidence acceptance surfaces."""

from __future__ import annotations

from typing import Any

from ..case_result import CaseResult
from ..c_api import (
    PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY,
    PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from .release_claims_owner_contracts import release_claims_surface_owner_payload
from ..runtime_contract_release import (
    RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE,
    RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
)


def build_runtime_release_candidate_claim_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"release-candidate-runtime-claim-abi"}
    ]
    return {
        "contract_id": RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "owner_contract": release_claims_surface_owner_payload(),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "claimability_semantics_release_policy_surface_contract_id": (
            RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID
        ),
        "final_claim_publication_deprecated_path_shutdown_surface_contract_id": (
            RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_release_candidate_claim_testing_boundary": (
            PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY
        ),
        "release_candidate_claim_snapshot_symbol": (
            "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing"
        ),
        "release_candidate_claim_snapshot_type": (
            "objc3_runtime_release_candidate_claim_snapshot"
        ),
        "conformance_publication_contract_id": (
            "objc3c.driver.conformance.report.publication.v1"
        ),
        "conformance_claim_operations_contract_id": (
            "objc3c.toolchain.conformance.claim.operations.v1"
        ),
        "release_evidence_operation_contract_id": (
            "objc3c.tooling.release.evidence.toolchain.operations.v1"
        ),
        "dashboard_status_publication_contract_id": (
            "objc3c.tooling.dashboard.status.publication.v1"
        ),
        "release_candidate_matrix_contract_id": (
            "objc3c.tooling.release.candidate.execution.matrix.v1"
        ),
        "claimed_profile_ids": [
            "core",
            "strict",
            "strict-concurrency",
            "strict-system",
        ],
        "targeted_profile_ids": ["strict", "strict-concurrency", "strict-system"],
        "runtime_claim_boundary_model": (
            "private-release-candidate-claim-snapshot-freezes-the-final-claim-publication-contract-set-and-retired-artifact-rejection-without-widening-the-public-runtime-header"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE],
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_current_release_evidence_owner_payload_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"current-release-evidence-owner-payload"}
    ]
    return {
        "contract_id": (
            RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID
        ),
        "owner_contract": release_claims_surface_owner_payload(),
        "runtime_release_candidate_claim_abi_surface_contract_id": (
            RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "private_release_candidate_evidence_testing_boundary": (
            PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY
        ),
        "release_candidate_evidence_snapshot_symbol": (
            "objc3_runtime_copy_release_candidate_evidence_state_for_testing"
        ),
        "release_candidate_evidence_snapshot_type": (
            "objc3_runtime_release_candidate_evidence_state_snapshot"
        ),
        "validation_artifact_name": "module.objc3-conformance-validation.json",
        "release_evidence_operation_artifact_name": (
            "module.objc3-release-evidence-operation.json"
        ),
        "dashboard_status_artifact_name": "module.objc3-dashboard-status.json",
        "advanced_feature_gate_artifact_name": (
            "module.objc3-advanced-feature-gate.json"
        ),
        "release_candidate_matrix_artifact_name": (
            "module.objc3-release-candidate-matrix.json"
        ),
        "current_release_evidence_owner_payload_model": (
            "private-release-candidate-evidence-snapshot-freezes-the-live-validation-release-evidence-dashboard-gate-matrix-and-retired-artifact-rejection-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_paths": [RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE],
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = [
    "RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID",
    "build_runtime_release_candidate_claim_abi_surface",
    "build_runtime_current_release_evidence_owner_payload_surface",
]
