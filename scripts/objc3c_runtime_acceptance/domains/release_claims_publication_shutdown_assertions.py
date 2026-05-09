"""Assertions for final release-claim publication shutdown cases."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from ..expectation_matching import expect
from .release_claims_retired_artifacts import retired_release_claim_artifacts_absent


EXPECTED_COMPILE_ARTIFACTS = [
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-conformance-publication.json",
    "module.objc3-conformance-report.json",
    "module.objc3-release-candidate-matrix.json",
]

EXPECTED_VALIDATE_ARTIFACTS = [
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-conformance-validation.json",
    "module.objc3-dashboard-status.json",
    "module.objc3-release-candidate-matrix.json",
    "module.objc3-release-evidence-operation.json",
]


def expect_live_compile_artifacts(compile_dir: Path, compile_artifacts: list[str]) -> None:
    expect(
        compile_artifacts == EXPECTED_COMPILE_ARTIFACTS,
        "expected native compile to publish only the live claim publication and release-candidate sidecars",
    )
    expect(
        retired_release_claim_artifacts_absent(compile_dir),
        "expected native compile to stop emitting every retired release-claim artifact filename",
    )


def expect_live_validate_artifacts(validate_dir: Path, validate_artifacts: list[str]) -> None:
    expect(
        validate_artifacts == EXPECTED_VALIDATE_ARTIFACTS,
        "expected conformance validation to publish the final post-publication release artifacts and no retired artifacts",
    )
    expect(
        retired_release_claim_artifacts_absent(validate_dir),
        "expected conformance validation to keep retired release-claim artifact paths shut down",
    )


def expect_final_publication_artifact_wiring(
    release_evidence_operation: dict[str, Any],
    advanced_feature_gate: dict[str, Any],
    release_candidate_matrix: dict[str, Any],
) -> None:
    expect(
        release_evidence_operation.get("operation_model")
        == "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set",
        "expected release evidence operation artifact to describe the final claim publication bundle instead of the retired dashboard-ready summary path",
    )
    expect(
        advanced_feature_gate.get("surface_kind") == "native-cli-validation"
        and release_candidate_matrix.get("surface_kind") == "native-cli-validation",
        "expected validation-emitted gate and matrix artifacts to identify the live validation publication path",
    )
    expect(
        advanced_feature_gate.get("dashboard_artifact_expected")
        == "module.objc3-dashboard-status.json"
        and release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected validation-emitted release artifacts to preserve the final live artifact wiring",
    )
    expect(
        release_candidate_matrix.get("matrix_model")
        == "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set",
        "expected release candidate matrix to describe the final claim publication artifact set instead of generic emitted artifacts",
    )
