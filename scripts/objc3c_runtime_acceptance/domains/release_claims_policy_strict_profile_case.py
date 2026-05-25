"""Strict-profile claim policy runtime acceptance case."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from ..case_result import CaseResult
from ..expectation_matching import expect
from ..fixture_compile_runner import run_fixture_compile
from ..paths import ROOT
from ..runtime_contract_release import RELEASE_CLAIMABLE_SURFACE_FIXTURE
from .release_claims_owner_contracts import release_claims_case_summary


def check_strict_profile_claim_implementation_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "strict-profile-claim-implementation"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    accepted_profiles = ["strict", "strict-concurrency"]
    rejected_profiles = ["strict-system"]
    summaries: list[dict[str, Any]] = []

    for profile in accepted_profiles:
        compile_dir = case_dir / profile
        result, _ = run_fixture_compile(
            fixture,
            compile_dir,
            extra_args=["--objc3-conformance-profile", profile],
            write_provenance=False,
        )
        diagnostic_text = (result.stderr or result.stdout).strip()
        expect(
            result.returncode == 0,
            f"expected {profile} conformance selection to compile",
        )
        summaries.append(
            {
                "profile": profile,
                "returncode": result.returncode,
                "diagnostic": diagnostic_text,
            }
        )

    for profile in rejected_profiles:
        compile_dir = case_dir / profile
        result, _ = run_fixture_compile(
            fixture,
            compile_dir,
            extra_args=["--objc3-conformance-profile", profile],
            write_provenance=False,
        )
        diagnostic_text = (result.stderr or result.stdout).strip()
        expect(
            result.returncode != 0,
            f"expected {profile} conformance selection to fail closed",
        )
        expect(
            f"unsupported --objc3-conformance-profile selection: {profile}"
            in diagnostic_text
            and "claimed profiles: core, strict, strict-concurrency"
            in diagnostic_text
            and "rejected built-in profiles: strict-system" in diagnostic_text,
            f"expected {profile} rejection to publish the centralized claim policy diagnostic",
        )
        summaries.append(
            {
                "profile": profile,
                "returncode": result.returncode,
                "diagnostic": diagnostic_text,
            }
        )

    return CaseResult(
        case_id="strict-profile-claim-implementation",
        probe="native-cli-selection-accepts-strict-profiles-and-fails-closed-for-strict-system",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "strict-profile-claim-implementation",
            {"profiles": summaries},
        ),
    )


__all__ = ["check_strict_profile_claim_implementation_case"]
