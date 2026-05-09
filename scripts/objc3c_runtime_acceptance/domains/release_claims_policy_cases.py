"""Release-claims policy and profile implementation runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


def check_claimability_semantics_release_policy_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "claimability-semantics-release-policy"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report_path = compile_dir / "module.objc3-conformance-report.json"
    publication_path = compile_dir / "module.objc3-conformance-publication.json"
    advanced_feature_gate_path = compile_dir / "module.objc3-advanced-feature-gate.json"
    release_candidate_matrix_path = (
        compile_dir / "module.objc3-release-candidate-matrix.json"
    )
    publication = json.loads(publication_path.read_text(encoding="utf-8"))

    validation_dir = case_dir / "validate"
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
        "expected conformance validation to succeed for the current claimed core profile",
    )
    validation_payload = json.loads(
        (validation_dir / "module.objc3-conformance-validation.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        advanced_feature_gate_path.read_text(encoding="utf-8")
    )
    release_candidate_matrix = json.loads(
        release_candidate_matrix_path.read_text(encoding="utf-8")
    )

    strict_compile_dir = case_dir / "strict"
    compile_fixture_with_args(
        fixture,
        strict_compile_dir,
        ["--objc3-conformance-profile", "strict"],
    )
    strict_publication = json.loads(
        (strict_compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )

    yaml_reject = run(
        [
            str(NATIVE_EXE),
            str(fixture),
            "--out-dir",
            str(case_dir / "yaml-reject"),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "yaml",
        ]
    )
    yaml_text = (yaml_reject.stderr or yaml_reject.stdout).strip()
    expect(
        yaml_reject.returncode != 0,
        "expected yaml conformance emission to fail closed",
    )
    expect(
        "claimed publication format: json; targeted release-evidence profiles: strict, strict-concurrency, strict-system"
        in yaml_text,
        "expected yaml emission rejection to publish the centralized format policy diagnostic",
    )

    expect(
        strict_publication.get("selected_profile") == "strict"
        and strict_publication.get("selected_profile_supported") is True,
        "expected strict profile selection to publish through the centralized claim policy",
    )
    expect(
        publication.get("supported_profile_ids") == ["core"]
        or publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"],
        "expected conformance publication to preserve a recognized live claim policy profile set",
    )
    expect(
        publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and publication.get("rejected_profile_ids") == [],
        "expected conformance publication to preserve the centralized live claim policy profile sets",
    )
    expect(
        validation_payload.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and validation_payload.get("rejected_profile_ids") == [],
        "expected conformance validation to preserve the centralized live claim policy profile sets",
    )
    expect(
        publication.get("advanced_feature_targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and validation_payload.get("advanced_feature_targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and advanced_feature_gate.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"]
        and release_candidate_matrix.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected publication, validation, gate, and matrix artifacts to preserve one centralized release-targeting policy",
    )

    return CaseResult(
        case_id="claimability-semantics-release-policy",
        probe="compile-publication-validation-and-fail-closed-operator-diagnostics",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "selected_profile": publication.get("selected_profile"),
            "supported_profile_ids": publication.get("supported_profile_ids"),
            "targeted_profile_ids": publication.get(
                "advanced_feature_targeted_profile_ids"
            ),
            "strict_selected_profile": strict_publication.get("selected_profile"),
            "yaml_reject_returncode": yaml_reject.returncode,
        },
    )


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
            and report.get("runtime_capability_report", {}).get("not_claimed_profile_ids")
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
        summary={"profiles": summaries},
    )


__all__ = [
    "check_claimability_semantics_release_policy_case",
    "check_strict_profile_claim_implementation_case",
]
