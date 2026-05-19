"""Strict-profile claim implementation runtime acceptance case."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from ..case_result import CaseResult
from ..expectation_matching import expect
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from ..runtime_contract_release import RELEASE_CLAIMABLE_SURFACE_FIXTURE
from .release_claims_owner_contracts import release_claims_case_summary


def check_strict_profile_claim_implementation_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "strict-profile-claim-implementation"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    claimed_profiles = ["strict", "strict-concurrency", "strict-system"]
    supported_profiles = ["core", "strict", "strict-concurrency", "strict-system"]
    summaries: list[dict[str, Any]] = []

    for profile in claimed_profiles:
        compile_dir = case_dir / profile
        compile_fixture_with_args(
            fixture,
            compile_dir,
            ["--objc3-conformance-profile", profile],
        )
        report_path = compile_dir / "module.objc3-conformance-report.json"
        publication_path = compile_dir / "module.objc3-conformance-publication.json"
        report = json.loads(report_path.read_text(encoding="utf-8"))
        publication = json.loads(publication_path.read_text(encoding="utf-8"))

        validation_dir = compile_dir / "validate"
        validation_dir.mkdir(parents=True, exist_ok=True)
        validation = run(
            [
                str(NATIVE_EXE),
                "--validate-objc3-conformance",
                str(report_path),
                "--out-dir",
                str(validation_dir),
                "--emit-prefix",
                "module",
                "--emit-objc3-conformance-format",
                "json",
            ]
        )
        expect(
            validation.returncode == 0,
            f"expected {profile} conformance validation to succeed",
        )
        validation_payload = json.loads(
            (validation_dir / "module.objc3-conformance-validation.json").read_text(
                encoding="utf-8"
            )
        )

        expect(
            publication.get("selected_profile") == profile
            and publication.get("selected_profile_supported") is True,
            f"expected publication to claim the selected {profile} profile",
        )
        expect(
            publication.get("supported_profile_ids") == supported_profiles
            and publication.get("rejected_profile_ids") == [],
            f"expected publication to preserve the fully claimed profile inventory for {profile}",
        )
        expect(
            validation_payload.get("selected_profile") == profile
            and validation_payload.get("selected_profile_supported") is True,
            f"expected validation to preserve the selected {profile} profile",
        )
        expect(
            validation_payload.get("supported_profile_ids") == supported_profiles
            and validation_payload.get("rejected_profile_ids") == [],
            f"expected validation to preserve the fully claimed profile inventory for {profile}",
        )
        expect(
            report.get("runtime_capability_report", {}).get("claimed_profile_ids")
            == supported_profiles
            and report.get("runtime_capability_report", {}).get(
                "not_claimed_profile_ids"
            )
            == [],
            f"expected runtime capability report to preserve the fully claimed profile inventory for {profile}",
        )
        summaries.append(
            {
                "profile": profile,
                "selected_profile": publication.get("selected_profile"),
                "supported_profile_ids": publication.get("supported_profile_ids"),
            }
        )

    return CaseResult(
        case_id="strict-profile-claim-implementation",
        probe="compile-publication-validation-and-runtime-capability-report-for-claimed-strict-profiles",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "strict-profile-claim-implementation",
            {"profiles": summaries},
        ),
    )


__all__ = ["check_strict_profile_claim_implementation_case"]
