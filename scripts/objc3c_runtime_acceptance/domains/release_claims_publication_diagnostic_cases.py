"""Release Claims publication diagnostic runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from .release_claims_owner_contracts import release_claims_case_summary
from .release_claims_retired_artifacts import (
    RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
    write_retired_release_claim_artifacts,
)
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


RETIRED_ARTIFACT_REJECTION_CASE_ID = "retired-artifact-rejection-contracts"
RETIRED_ARTIFACT_REJECTION_DIAGNOSTIC_FRAGMENT = (
    "deprecated claim/scaffold compatibility sidecar(s) detected"
)


def check_retired_artifact_rejection_contracts_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / RETIRED_ARTIFACT_REJECTION_CASE_ID
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_retired_dir = case_dir / "compile-retired-artifacts"
    compile_retired_dir.mkdir(parents=True, exist_ok=True)
    write_retired_release_claim_artifacts(compile_retired_dir)
    compile_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(compile_retired_dir),
            "--emit-prefix",
            "module",
        ]
    )
    compile_reject_text = (compile_reject.stderr or compile_reject.stdout).strip()
    expect(
        compile_reject.returncode != 0,
        "expected native compile to fail closed when retired release-claim artifacts are present",
    )
    expect(
        RETIRED_ARTIFACT_REJECTION_DIAGNOSTIC_FRAGMENT in compile_reject_text,
        "expected native compile rejection to publish the retired artifact rejection diagnostic",
    )

    validate_compile_dir = case_dir / "validate-source"
    compile_fixture_with_args(fixture, validate_compile_dir)
    report_path = validate_compile_dir / "module.objc3-conformance-report.json"
    write_retired_release_claim_artifacts(validate_compile_dir)
    validate_dir = case_dir / "validate-output"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate_reject = run(
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
    validate_reject_text = (validate_reject.stderr or validate_reject.stdout).strip()
    expect(
        validate_reject.returncode != 0,
        "expected conformance validation to fail closed when retired release-claim artifacts are present next to the validated report",
    )
    expect(
        RETIRED_ARTIFACT_REJECTION_DIAGNOSTIC_FRAGMENT in validate_reject_text,
        "expected conformance validation rejection to publish the retired artifact rejection diagnostic",
    )

    return CaseResult(
        case_id=RETIRED_ARTIFACT_REJECTION_CASE_ID,
        probe="compile-and-validate-with-retired-release-claim-artifacts-present",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            RETIRED_ARTIFACT_REJECTION_CASE_ID,
            {
                "retired_artifact_filenames": RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
                "compile_reject_returncode": compile_reject.returncode,
                "validate_reject_returncode": validate_reject.returncode,
            },
        ),
    )


__all__ = [
    "RETIRED_ARTIFACT_REJECTION_CASE_ID",
    "check_retired_artifact_rejection_contracts_case",
]
