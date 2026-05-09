"""Release Claims publication shutdown runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from .release_claims_publication_shutdown_assertions import (
    expect_final_publication_artifact_wiring,
    expect_live_compile_artifacts,
    expect_live_validate_artifacts,
)
from .release_claims_owner_contracts import release_claims_case_summary
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


def check_final_claim_publication_deprecated_path_shutdown_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "final-claim-publication-deprecated-path-shutdown"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    compile_artifacts = sorted(path.name for path in compile_dir.glob("module.objc3-*.json"))
    expect_live_compile_artifacts(compile_dir, compile_artifacts)

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
    expect_live_validate_artifacts(validate_dir, validate_artifacts)

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
    expect_final_publication_artifact_wiring(
        release_evidence_operation, advanced_feature_gate, release_candidate_matrix
    )

    return CaseResult(
        case_id="final-claim-publication-deprecated-path-shutdown",
        probe="compile-validate-and-inspect-the-final-claim-publication-bundle",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "final-claim-publication-deprecated-path-shutdown",
            {
                "compile_artifacts": compile_artifacts,
                "validate_artifacts": validate_artifacts,
                "validation_surface_kind": advanced_feature_gate.get("surface_kind"),
            },
        ),
    )


__all__ = [
    "check_final_claim_publication_deprecated_path_shutdown_case",
]
