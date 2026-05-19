"""Assertions for current release evidence owner payload runtime acceptance."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect
from ..c_api import PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY
from ..runtime_contract_release import (
    RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
)


EXPECTED_CURRENT_RELEASE_EVIDENCE_ARTIFACTS = [
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-conformance-validation.json",
    "module.objc3-dashboard-status.json",
    "module.objc3-release-candidate-matrix.json",
    "module.objc3-release-evidence-operation.json",
]


CURRENT_RELEASE_EVIDENCE_MANIFEST_SURFACE_KEY = (
    "runtime_current_release_evidence_owner_payload_surface"
)


def expect_current_release_evidence_owner_payload_surface(
    surface: dict[str, Any],
) -> None:
    expect(
        isinstance(surface, dict),
        "expected compiled fixture manifest to publish the current release evidence owner payload surface",
    )
    expect(
        surface.get("contract_id")
        == RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID,
        "expected compiled fixture manifest to publish the current release evidence owner payload surface contract",
    )
    expect(
        surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        "expected current release evidence owner payload surface to depend on the release-candidate claim ABI surface",
    )
    expect(
        surface.get("private_release_candidate_evidence_testing_boundary")
        == PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY,
        "expected current release evidence owner payload surface to preserve the private evidence snapshot boundary",
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
        "expected current release evidence owner payload surface to publish the current artifact inventory",
    )
    expect(
        surface.get("authoritative_probe_paths")
        == ["tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"],
        "expected current release evidence owner payload surface to publish the authoritative runtime probe path",
    )


def expect_current_release_validate_artifacts(validate_artifacts: list[str]) -> None:
    expect(
        validate_artifacts == EXPECTED_CURRENT_RELEASE_EVIDENCE_ARTIFACTS,
        "expected validation output to preserve the current release evidence artifact inventory",
    )


def expect_current_release_probe_payload(payload: dict[str, Any]) -> None:
    expect(
        payload.get("copy_status") == 0
        and payload.get("validation_artifact_ready") == 1
        and payload.get("release_evidence_operation_ready") == 1
        and payload.get("dashboard_status_ready") == 1
        and payload.get("advanced_feature_gate_ready") == 1
        and payload.get("release_candidate_matrix_ready") == 1
        and payload.get("deprecated_paths_shutdown") == 1
        and payload.get("deterministic") == 1,
        "expected current release evidence runtime probe to publish a ready deterministic owner payload snapshot",
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
        "expected current release evidence runtime probe to preserve the current artifact inventory",
    )
