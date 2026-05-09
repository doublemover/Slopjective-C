"""Release Claims publication diagnostic runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from .release_claims_deprecated_sidecars import write_deprecated_claim_sidecars
from ..runtime_contract_release import (
    DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


def check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)

    compile_deprecated_dir = case_dir / "compile-deprecated"
    compile_deprecated_dir.mkdir(parents=True, exist_ok=True)
    write_deprecated_claim_sidecars(compile_deprecated_dir)
    compile_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(compile_deprecated_dir),
            "--emit-prefix",
            "module",
        ]
    )
    compile_reject_text = (compile_reject.stderr or compile_reject.stdout).strip()
    expect(
        compile_reject.returncode != 0,
        "expected native compile to fail closed when deprecated claim/scaffold sidecars are present",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in compile_reject_text,
        "expected native compile rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    validate_compile_dir = case_dir / "validate-source"
    compile_fixture_with_args(fixture, validate_compile_dir)
    report_path = validate_compile_dir / "module.objc3-conformance-report.json"
    write_deprecated_claim_sidecars(validate_compile_dir)
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
        "expected conformance validation to fail closed when deprecated claim/scaffold sidecars are present next to the validated report",
    )
    expect(
        "deprecated claim/scaffold compatibility sidecar(s) detected"
        in validate_reject_text,
        "expected conformance validation rejection to publish the deprecated sidecar compatibility diagnostic",
    )

    return CaseResult(
        case_id="scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
        probe="compile-and-validate-with-deprecated-claim-sidecars-present",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "deprecated_sidecar_filenames": DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES,
            "compile_reject_returncode": compile_reject.returncode,
            "validate_reject_returncode": validate_reject.returncode,
        },
    )


__all__ = [
    "check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case",
]
