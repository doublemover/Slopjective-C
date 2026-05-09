"""Assertions for release-claims residual non-claimable gap surfaces."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


EXPECTED_OPTIONAL_GAP_IDS = ["throws", "async-await", "actors", "blocks", "arc"]
EXPECTED_CLAIMED_PROFILES = [
    "core",
    "strict",
    "strict-concurrency",
    "strict-system",
]


def expect_residual_non_claimable_gap_surfaces(
    report: dict[str, Any],
    publication: dict[str, Any],
    advanced_feature_gate: dict[str, Any],
    release_candidate_matrix: dict[str, Any],
    runtime_capability_report: dict[str, Any],
) -> None:
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
        and publication.get("supported_profile_ids") == EXPECTED_CLAIMED_PROFILES,
        "expected native publication to publish all currently claimable profiles while defaulting to core",
    )
    expect(
        publication.get("rejected_profile_ids") == [],
        "expected native publication to stop reporting any rejected built-in profiles",
    )
    expect(
        runtime_capability_report.get("claimed_profile_ids")
        == EXPECTED_CLAIMED_PROFILES
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
        == EXPECTED_CLAIMED_PROFILES[1:]
        and release_candidate_matrix.get("targeted_profile_ids")
        == EXPECTED_CLAIMED_PROFILES[1:],
        "expected release gate sidecars to keep targeting the advanced strict profiles after they become claimable",
    )
    expect(
        release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json",
        "expected release-candidate matrix to stay coupled to the advanced feature gate artifact",
    )


__all__ = [
    "EXPECTED_CLAIMED_PROFILES",
    "EXPECTED_OPTIONAL_GAP_IDS",
    "expect_residual_non_claimable_gap_surfaces",
]
