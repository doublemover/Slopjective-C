"""Claimability release-policy runtime acceptance case."""

from __future__ import annotations

import json
from pathlib import Path

from ..case_result import CaseResult
from ..expectation_matching import expect
from ..fixture_compilation import compile_fixture_with_args
from ..fixture_compile_runner import run_fixture_compile
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from ..runtime_contract_release import RELEASE_CLAIMABLE_SURFACE_FIXTURE
from .release_claims_owner_contracts import release_claims_case_summary


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

    strict_reject_dir = case_dir / "strict-reject"
    strict_reject, _ = run_fixture_compile(
        fixture,
        strict_reject_dir,
        extra_args=["--objc3-conformance-profile", "strict"],
        write_provenance=False,
    )
    strict_reject_text = (strict_reject.stderr or strict_reject.stdout).strip()

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
        publication.get("supported_profile_ids") == ["core"]
        and publication.get("rejected_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected conformance publication to preserve the centralized core-only claim policy profile sets",
    )
    expect(
        validation_payload.get("supported_profile_ids") == ["core"]
        and validation_payload.get("rejected_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "expected conformance validation to preserve the centralized core-only claim policy profile sets",
    )
    expect(
        strict_reject.returncode != 0
        and "unsupported --objc3-conformance-profile selection: strict"
        in strict_reject_text
        and "claimed profiles: core" in strict_reject_text,
        "expected strict profile selection to fail closed through the centralized claim policy",
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
        summary=release_claims_case_summary(
            "claimability-semantics-release-policy",
            {
                "selected_profile": publication.get("selected_profile"),
                "supported_profile_ids": publication.get("supported_profile_ids"),
                "targeted_profile_ids": publication.get(
                    "advanced_feature_targeted_profile_ids"
                ),
                "strict_reject_returncode": strict_reject.returncode,
                "yaml_reject_returncode": yaml_reject.returncode,
            },
        ),
    )


__all__ = ["check_claimability_semantics_release_policy_case"]
