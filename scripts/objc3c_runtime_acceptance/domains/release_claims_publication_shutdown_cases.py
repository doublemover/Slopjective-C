"""Release Claims publication shutdown runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..case_result import CaseResult
from ..commands import run
from ..core import (
    DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
    NATIVE_EXE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    compile_fixture_with_args,
    expect,
)


def check_final_claim_publication_deprecated_path_shutdown_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "final-claim-publication-deprecated-path-shutdown"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    compile_artifacts = sorted(path.name for path in compile_dir.glob("module.objc3-*.json"))
    expect(
        compile_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-report.json",
            "module.objc3-release-candidate-matrix.json",
        ],
        "expected native compile to publish only the live claim publication and release-candidate sidecars",
    )
    expect(
        all(
            not (compile_dir / filename).exists()
            for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES
        ),
        "expected native compile to stop emitting every deprecated claim/scaffold sidecar filename",
    )

    report_path = compile_dir / "module.objc3-conformance-report.json"
    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to publish the full final claim publication bundle",
    )

    validate_artifacts = sorted(path.name for path in validate_dir.glob("module.objc3-*.json"))
    expect(
        validate_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-dashboard-status.json",
            "module.objc3-release-candidate-matrix.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected conformance validation to publish the final post-publication release artifacts and no deprecated sidecars",
    )
    expect(
        all(
            not (validate_dir / filename).exists()
            for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES
        ),
        "expected conformance validation to keep deprecated claim/scaffold sidecar paths shut down",
    )

    release_evidence_operation = json.loads(
        (validate_dir / "module.objc3-release-evidence-operation.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (validate_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (validate_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
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
        "expected release candidate matrix to describe the final claim publication artifact set instead of emitted sidecars generically",
    )

    return CaseResult(
        case_id="final-claim-publication-deprecated-path-shutdown",
        probe="compile-validate-and-inspect-the-final-claim-publication-bundle",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "compile_artifacts": compile_artifacts,
            "validate_artifacts": validate_artifacts,
            "validation_surface_kind": advanced_feature_gate.get("surface_kind"),
        },
    )


__all__ = [
    "check_final_claim_publication_deprecated_path_shutdown_case",
]
