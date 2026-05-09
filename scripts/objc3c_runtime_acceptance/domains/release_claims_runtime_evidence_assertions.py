"""Assertions for final release evidence runtime acceptance."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect
from ..c_api import PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY
from ..runtime_contract_release import (
    RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
)


EXPECTED_FINAL_RELEASE_EVIDENCE_ARTIFACTS = [
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-conformance-validation.json",
    "module.objc3-dashboard-status.json",
    "module.objc3-release-candidate-matrix.json",
    "module.objc3-release-evidence-operation.json",
]


def expect_final_release_implementation_surface(surface: dict[str, Any]) -> None:
    expect(
        isinstance(surface, dict),
        "expected compiled fixture manifest to publish runtime_final_release_evidence_descaffolding_implementation_surface",
    )
    expect(
        surface.get("contract_id")
        == RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the final release evidence descaffolding implementation surface contract",
    )
    expect(
        surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected final release evidence implementation surface to depend on the release-candidate claim ABI surface",
    )
    expect(
        surface.get("private_release_candidate_evidence_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
        "expected final release evidence implementation surface to preserve the private evidence snapshot boundary",
    )
    expect(
        surface.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and surface.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and surface.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and surface.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and surface.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence implementation surface to publish the final artifact inventory",
    )


def expect_final_release_validate_artifacts(validate_artifacts: list[str]) -> None:
    expect(
        validate_artifacts == EXPECTED_FINAL_RELEASE_EVIDENCE_ARTIFACTS,
        "expected validation output to preserve the final release evidence artifact inventory",
    )


def expect_final_release_probe_payload(payload: dict[str, Any]) -> None:
    expect(
        payload.get("copy_status") == 0
        and payload.get("validation_artifact_ready") == 1
        and payload.get("release_evidence_operation_ready") == 1
        and payload.get("dashboard_status_ready") == 1
        and payload.get("advanced_feature_gate_ready") == 1
        and payload.get("release_candidate_matrix_ready") == 1
        and payload.get("deprecated_paths_shutdown") == 1
        and payload.get("deterministic") == 1,
        "expected final release evidence runtime probe to publish a ready deterministic implementation snapshot",
    )
    expect(
        payload.get("validation_artifact_name")
        == "module.objc3-conformance-validation.json"
        and payload.get("release_evidence_operation_artifact_name")
        == "module.objc3-release-evidence-operation.json"
        and payload.get("dashboard_status_artifact_name")
        == "module.objc3-dashboard-status.json"
        and payload.get("advanced_feature_gate_artifact_name")
        == "module.objc3-advanced-feature-gate.json"
        and payload.get("release_candidate_matrix_artifact_name")
        == "module.objc3-release-candidate-matrix.json",
        "expected final release evidence runtime probe to preserve the final artifact inventory",
    )
