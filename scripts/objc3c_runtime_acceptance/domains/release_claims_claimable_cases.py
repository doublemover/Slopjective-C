"""Release-claims claimable feature source runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..case_result import CaseResult
from ..core import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    compile_fixture_with_args,
    expect,
)


def check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "claimable-surface-residual-non-claimable-gaps-source-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report = json.loads(
        (compile_dir / "module.objc3-conformance-report.json").read_text(
            encoding="utf-8"
        )
    )
    publication = json.loads(
        (compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (compile_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (compile_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    runtime_capability_report = report.get("runtime_capability_report", {})

    expected_optional_gap_ids = ["throws", "async-await", "actors", "blocks", "arc"]
    expected_claimed_profiles = [
        "core",
        "strict",
        "strict-concurrency",
        "strict-system",
    ]

    expect(
        report.get("contract_id") == "objc3c.versioned.conformance.report.lowering.v1",
        "expected native compile to publish the versioned conformance report sidecar",
    )
    expect(
        publication.get("contract_id")
        == "objc3c.driver.conformance.report.publication.v1",
        "expected native compile to publish the conformance publication sidecar",
    )
    expect(
        runtime_capability_report.get("contract_id")
        == "objc3c.runtime.capability.reporting.v1",
        "expected conformance report to embed the runtime capability report",
    )
    expect(
        advanced_feature_gate.get("contract_id")
        == "objc3c.tooling.integrated.advanced.feature.gate.v1",
        "expected native compile to publish the advanced feature gate sidecar",
    )
    expect(
        release_candidate_matrix.get("contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1",
        "expected native compile to publish the release-candidate matrix sidecar",
    )
    expect(
        publication.get("selected_profile") == "core"
        and publication.get("selected_profile_supported") is True
        and publication.get("supported_profile_ids") == expected_claimed_profiles,
        "expected native publication to publish all currently claimable profiles while defaulting to core",
    )
    expect(
        publication.get("rejected_profile_ids") == [],
        "expected native publication to stop reporting any rejected built-in profiles",
    )
    expect(
        runtime_capability_report.get("claimed_profile_ids") == expected_claimed_profiles
        and runtime_capability_report.get("not_claimed_profile_ids") == [],
        "expected runtime capability report to publish all currently claimable profiles and no residual profile gaps",
    )
    expect(
        report.get("unsupported_feature_claim_ids")
        == [
            "unsupported:strictness-selection",
            "unsupported:strict-concurrency-selection",
            "unsupported:throws",
            "unsupported:async-await",
            "unsupported:actors",
            "unsupported:blocks",
            "unsupported:arc",
        ],
        "expected versioned conformance report to publish the residual unsupported claim inventory",
    )
    optional_feature_statuses = {
        entry.get("id"): entry.get("status")
        for entry in runtime_capability_report.get("optional_features", [])
        if isinstance(entry, dict)
    }
    expect(
        optional_feature_statuses
        == {
            "throws": "not-claimed",
            "async-await": "not-claimed",
            "actors": "not-claimed",
            "blocks": "not-claimed",
            "arc": "not-claimed",
        },
        "expected runtime capability report to publish the residual optional non-claimable gaps",
    )
    expect(
        advanced_feature_gate.get("targeted_profile_ids")
        == expected_claimed_profiles[1:]
        and release_candidate_matrix.get("targeted_profile_ids")
        == expected_claimed_profiles[1:],
        "expected release gate sidecars to keep targeting the advanced strict profiles after they become claimable",
    )
    expect(
        release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected release-candidate matrix to stay coupled to the advanced feature gate artifact",
    )

    return CaseResult(
        case_id="claimable-surface-residual-non-claimable-gaps-source-surface",
        probe="compile-conformance-report-publication-runtime-capability-and-release-gate-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "claimed_profiles": runtime_capability_report.get("claimed_profile_ids"),
            "selected_profile": publication.get("selected_profile"),
            "unsupported_feature_claim_ids": report.get(
                "unsupported_feature_claim_ids"
            ),
            "optional_non_claimable_features": expected_optional_gap_ids,
        },
    )


def check_strict_profile_feature_claim_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "strict-profile-feature-claim-source-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)

    report = json.loads(
        (compile_dir / "module.objc3-conformance-report.json").read_text(
            encoding="utf-8"
        )
    )
    publication = json.loads(
        (compile_dir / "module.objc3-conformance-publication.json").read_text(
            encoding="utf-8"
        )
    )
    advanced_feature_gate = json.loads(
        (compile_dir / "module.objc3-advanced-feature-gate.json").read_text(
            encoding="utf-8"
        )
    )
    release_candidate_matrix = json.loads(
        (compile_dir / "module.objc3-release-candidate-matrix.json").read_text(
            encoding="utf-8"
        )
    )
    feature_claim_truth_surface = report.get("feature_claim_truth_surface", {})
    compatibility_semantics = report.get(
        "compatibility_strictness_claim_semantics", {}
    )
    expected_targeted_profiles = ["strict", "strict-concurrency", "strict-system"]

    expect(
        feature_claim_truth_surface.get("contract_id")
        == "objc3c.feature.claim.strictness.truth.surface.v1",
        "expected conformance report to embed the strictness and feature-claim truth surface",
    )
    expect(
        feature_claim_truth_surface.get("supported_selection_surface_ids")
        == [
            "selection:language-version",
            "selection:language-profile",
        ],
        "expected feature-claim truth surface to preserve the live supported selection set",
    )
    expect(
        feature_claim_truth_surface.get("unsupported_selection_surface_ids")
        == [
            "selection:strictness",
            "selection:strict-concurrency",
            "selection:canonical-rejection-diagnostics",
        ],
        "expected feature-claim truth surface to preserve the fail-closed selection set",
    )
    expect(
        feature_claim_truth_surface.get("strictness_selection_supported") is False
        and feature_claim_truth_surface.get("strict_concurrency_selection_supported")
        is False
        and feature_claim_truth_surface.get("feature_macro_surface_supported")
        is False
        and feature_claim_truth_surface.get("claim_truth_fail_closed") is True,
        "expected feature-claim truth surface to publish fail-closed strictness, strict-concurrency, and macro behavior",
    )
    expect(
        compatibility_semantics.get("contract_id")
        == "objc3c.compatibility.strictness.claim.semantics.v1",
        "expected conformance report to embed the compatibility/strictness claim semantics surface",
    )
    expect(
        compatibility_semantics.get("rejection_model")
        == "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed",
        "expected compatibility semantics to preserve the strict-profile rejection model",
    )
    expect(
        compatibility_semantics.get("fail_closed") is True
        and compatibility_semantics.get("strictness_selection_rejection_semantics_landed")
        is True
        and compatibility_semantics.get("feature_macro_claim_suppression_semantics_landed")
        is True
        and compatibility_semantics.get("ready_for_lowering_and_runtime") is True,
        "expected compatibility semantics to preserve a ready fail-closed strict-profile boundary",
    )
    expect(
        publication.get("supported_profile_ids")
        == ["core", "strict", "strict-concurrency", "strict-system"]
        and publication.get("rejected_profile_ids") == [],
        "expected publication to preserve the full strict-profile claim set once claim implementation lands",
    )
    expect(
        publication.get("advanced_feature_targeted_profile_ids")
        == expected_targeted_profiles
        and advanced_feature_gate.get("targeted_profile_ids")
        == expected_targeted_profiles
        and release_candidate_matrix.get("targeted_profile_ids")
        == expected_targeted_profiles,
        "expected publication, advanced feature gate, and release-candidate matrix to preserve one strict-profile targeting set",
    )

    return CaseResult(
        case_id="strict-profile-feature-claim-source-surface",
        probe="compile-conformance-report-publication-and-release-gate-targeting-sidecars",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "supported_selection_surface_ids": feature_claim_truth_surface.get(
                "supported_selection_surface_ids"
            ),
            "unsupported_selection_surface_ids": feature_claim_truth_surface.get(
                "unsupported_selection_surface_ids"
            ),
            "targeted_profile_ids": expected_targeted_profiles,
            "rejection_model": compatibility_semantics.get("rejection_model"),
        },
    )


__all__ = [
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case",
    "check_strict_profile_feature_claim_source_surface_case",
]
