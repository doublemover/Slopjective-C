from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from build_full_envelope_claimability_dashboard_inputs import (
    ClaimabilityDashboardInputs,
    load_claimability_inputs,
)
from build_full_envelope_claimability_dashboard_io import expect

try:
    from build_full_envelope_claimability_contracts import (
        ACCEPTANCE_FAMILIES,
        DASHBOARD_DECISION_FIELDS,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
        public_claim_class_for_rollout,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        ACCEPTANCE_FAMILIES,
        DASHBOARD_DECISION_FIELDS,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
        public_claim_class_for_rollout,
    )


def acceptance_signal(family: str, report: dict[str, Any]) -> str | None:
    if family == "public-conformance":
        return report.get("public_status")
    if family == "performance-governance":
        return report.get("release_status")
    if family in {"release-foundation", "release-operations"}:
        return "publishable" if any(report.get(field) for field in RELEASE_ARTIFACT_FIELDS) else "missing-artifacts"
    if family == "distribution-credibility":
        return report.get("trust_state")
    return next(
        spec.claim_signal
        for spec in ACCEPTANCE_FAMILIES
        if spec.family == family
    )


def build_acceptance_matrix(
    inputs: ClaimabilityDashboardInputs,
) -> list[dict[str, str | None]]:
    acceptance_matrix = [
        {
            "family": spec.family,
            "status": inputs.integration_reports[spec.report_path].get("status"),
            "claim_signal": acceptance_signal(
                spec.family,
                inputs.integration_reports[spec.report_path],
            ),
        }
        for spec in ACCEPTANCE_FAMILIES
    ]
    expect(
        {row["family"] for row in acceptance_matrix}
        == set(inputs.contract["required_acceptance_matrix_families"]),
        "dashboard acceptance matrix family contract drifted",
    )
    expect(
        all(isinstance(row["status"], str) and isinstance(row["claim_signal"], str) for row in acceptance_matrix),
        "dashboard acceptance matrix must carry string status and claim_signal values",
    )
    return acceptance_matrix


def build_release_artifacts(inputs: ClaimabilityDashboardInputs) -> dict[str, Any]:
    return {
        "release_manifest_path": inputs.release_foundation.get("release_manifest_path"),
        "published_sbom": inputs.release_foundation.get("published_sbom"),
        "published_attestation": inputs.release_foundation.get("published_attestation"),
        "update_manifest_path": inputs.release_operations.get("update_manifest_path"),
        "upgrade_support_report": inputs.release_operations.get("upgrade_support_report"),
        "channel_catalog": inputs.release_operations.get("channel_catalog"),
        "trust_report_json": inputs.distribution.get("trust_report_json"),
    }


def dashboard_release_blocker_projection(
    inputs: ClaimabilityDashboardInputs,
    public_claim_class: str,
) -> dict[str, Any]:
    projection = inputs.release_blockers.get("dashboard_release_blocker_projection")
    expect(
        isinstance(projection, dict),
        "release blocker summary missing dashboard_release_blocker_projection",
    )
    expect(
        projection.get("current_rollout_class")
        == inputs.rollout["current_rollout_class"],
        "dashboard release-blocker projection rollout class drifted",
    )
    expect(
        projection.get("public_claim_class") == public_claim_class,
        "dashboard release-blocker projection public claim class drifted",
    )
    if projection.get("blocks_production_strength_claim"):
        expect(
            projection["blocker"] in inputs.release_blockers["triggered_blockers"],
            "dashboard release-blocker projection did not trigger its blocker",
        )
    return projection


def build_dashboard_payload(inputs: ClaimabilityDashboardInputs) -> dict[str, Any]:
    public_claim_class = public_claim_class_for_rollout(
        inputs.rollout["current_rollout_class"],
        inputs.rollout["production_strength_claimable"],
    )
    release_blocker_projection = dashboard_release_blocker_projection(
        inputs,
        public_claim_class,
    )
    acceptance_matrix = build_acceptance_matrix(inputs)
    release_artifacts = build_release_artifacts(inputs)
    dashboard_payload = {
        "contract_id": "objc3c.full_envelope.claimability.dashboard.summary.v1",
        "status": "PASS",
        "current_rollout_class": inputs.rollout["current_rollout_class"],
        "production_strength_claimable": inputs.rollout["production_strength_claimable"],
        "triggered_reasons": inputs.rollout["triggered_reasons"],
        "source_summaries": list(inputs.source_summaries.keys()),
        "source_reports": list(inputs.integration_reports.keys()),
        "public_claim_class": public_claim_class,
        "support_row_counts_by_class": inputs.support_matrix["support_row_counts_by_class"],
        "triggered_release_blockers": inputs.release_blockers["triggered_blockers"],
        "dashboard_release_blocker_projection": release_blocker_projection,
        "acceptance_matrix": acceptance_matrix,
        "release_artifacts": release_artifacts,
    }

    for required_key in inputs.schema.get("required", []):
        expect(required_key in dashboard_payload, f"dashboard payload missing schema-required key: {required_key}")
    for required_key in inputs.contract["required_dashboard_fields"]:
        expect(required_key in dashboard_payload, f"dashboard payload missing contract-required key: {required_key}")
    for required_key in release_blocker_projection["required_dashboard_fields"]:
        expect(required_key in dashboard_payload, f"dashboard payload missing release-blocker-required key: {required_key}")
    expect(
        set(DASHBOARD_DECISION_FIELDS).issubset(dashboard_payload),
        "dashboard payload missing source-owned decision fields",
    )
    expect(
        set(inputs.contract["required_release_artifact_fields"]) == set(release_artifacts),
        "dashboard release artifact field contract drifted",
    )
    return dashboard_payload


def build_public_payload(
    *,
    dashboard_payload: dict[str, Any],
    contract: dict[str, Any],
    report_markdown_path: Path,
) -> dict[str, Any]:
    release_blocker_projection = dashboard_payload["dashboard_release_blocker_projection"]
    public_payload = {
        "contract_id": "objc3c.full_envelope.claimability.public.summary.v1",
        "status": "PASS",
        "current_rollout_class": dashboard_payload["current_rollout_class"],
        "public_claim_class": dashboard_payload["public_claim_class"],
        "production_strength_claimable": dashboard_payload["production_strength_claimable"],
        "triggered_reason_count": len(dashboard_payload["triggered_reasons"]),
        "dashboard_release_blocker": release_blocker_projection["blocker"],
        "dashboard_blocks_production_strength_claim": release_blocker_projection[
            "blocks_production_strength_claim"
        ],
        "report_markdown_path": repo_rel(report_markdown_path),
    }
    for required_key in contract["required_public_summary_fields"]:
        expect(required_key in public_payload, f"public summary missing contract-required key: {required_key}")
    for required_key in release_blocker_projection["required_public_summary_fields"]:
        expect(required_key in public_payload, f"public summary missing release-blocker-required key: {required_key}")
    expect(
        set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(public_payload),
        "public summary missing source-owned decision fields",
    )
    return public_payload


def build_report_text(
    *,
    dashboard_payload: dict[str, Any],
    public_claim_class: str,
) -> str:
    release_blocker_projection = dashboard_payload["dashboard_release_blocker_projection"]
    return (
        "# objc3c Full-Envelope Claimability Report\n\n"
        f"- Rollout class: `{dashboard_payload['current_rollout_class']}`\n"
        f"- Public claim class: `{public_claim_class}`\n"
        f"- Production-strength claimable: `{dashboard_payload['production_strength_claimable']}`\n"
        f"- Dashboard release blocker: `{release_blocker_projection['blocker']}`\n"
        f"- Dashboard blocks production-strength claim: `{release_blocker_projection['blocks_production_strength_claim']}`\n"
        f"- Triggered release blockers: `{len(dashboard_payload['triggered_release_blockers'])}`\n"
        f"- Triggered reasons: `{len(dashboard_payload['triggered_reasons'])}`\n\n"
        "## Acceptance Matrix\n\n"
        + "\n".join(
            f"- `{row['family']}`: status `{row['status']}`, signal `{row['claim_signal']}`"
            for row in dashboard_payload["acceptance_matrix"]
        )
        + "\n"
    )


__all__ = (
    "acceptance_signal",
    "build_acceptance_matrix",
    "build_dashboard_payload",
    "build_public_payload",
    "build_release_artifacts",
    "build_report_text",
    "dashboard_release_blocker_projection",
    "load_claimability_inputs",
)
