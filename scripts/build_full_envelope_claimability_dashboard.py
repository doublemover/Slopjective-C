#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel, resolve_repo_path
try:
    from build_full_envelope_claimability_contracts import (
        ACCEPTANCE_FAMILIES,
        CLAIMABILITY_REPORT_MD,
        CLAIM_POLICY_SUMMARY,
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
        SUPPORT_MATRIX_SUMMARY,
        public_claim_class_for_rollout,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        ACCEPTANCE_FAMILIES,
        CLAIMABILITY_REPORT_MD,
        CLAIM_POLICY_SUMMARY,
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
        SUPPORT_MATRIX_SUMMARY,
        public_claim_class_for_rollout,
    )

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json"
SCHEMA_PATH = ROOT / "schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_REPORT_DIR = ROOT / "tmp/reports/full-envelope-claimability"
OUT_ARTIFACT_DIR = ROOT / "tmp/artifacts/full-envelope-claimability/report"
DASHBOARD_JSON = resolve_repo_path(DASHBOARD_SUMMARY)
PUBLIC_JSON = resolve_repo_path(PUBLIC_SUMMARY)
REPORT_MD = resolve_repo_path(CLAIMABILITY_REPORT_MD)


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


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




def main() -> int:
    contract = read_json(CONTRACT_PATH)
    schema = read_json(SCHEMA_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    source_summaries = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_source_summaries"]
    }
    integration_reports = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_integration_reports"]
    }

    support_matrix = source_summaries[SUPPORT_MATRIX_SUMMARY]
    claim_policy = source_summaries[CLAIM_POLICY_SUMMARY]
    release_blockers = source_summaries[RELEASE_BLOCKER_SUMMARY]
    rollout = source_summaries[ROLLOUT_READINESS_SUMMARY]

    release_foundation = integration_reports[RELEASE_FOUNDATION_SUMMARY]
    release_operations = integration_reports[RELEASE_OPERATIONS_SUMMARY]
    distribution = integration_reports[DISTRIBUTION_CREDIBILITY_SUMMARY]

    public_claim_class = public_claim_class_for_rollout(
        rollout["current_rollout_class"],
        rollout["production_strength_claimable"],
    )
    dashboard_release_blocker_projection = release_blockers.get(
        "dashboard_release_blocker_projection"
    )
    expect(
        isinstance(dashboard_release_blocker_projection, dict),
        "release blocker summary missing dashboard_release_blocker_projection",
    )
    expect(
        dashboard_release_blocker_projection.get("current_rollout_class")
        == rollout["current_rollout_class"],
        "dashboard release-blocker projection rollout class drifted",
    )
    expect(
        dashboard_release_blocker_projection.get("public_claim_class")
        == public_claim_class,
        "dashboard release-blocker projection public claim class drifted",
    )
    if dashboard_release_blocker_projection.get("blocks_production_strength_claim"):
        expect(
            dashboard_release_blocker_projection["blocker"]
            in release_blockers["triggered_blockers"],
            "dashboard release-blocker projection did not trigger its blocker",
        )

    acceptance_matrix = [
        {
            "family": spec.family,
            "status": integration_reports[spec.report_path].get("status"),
            "claim_signal": acceptance_signal(spec.family, integration_reports[spec.report_path]),
        }
        for spec in ACCEPTANCE_FAMILIES
    ]
    expect(
        {row["family"] for row in acceptance_matrix}
        == set(contract["required_acceptance_matrix_families"]),
        "dashboard acceptance matrix family contract drifted",
    )
    expect(
        all(isinstance(row["status"], str) and isinstance(row["claim_signal"], str) for row in acceptance_matrix),
        "dashboard acceptance matrix must carry string status and claim_signal values",
    )
    release_artifacts = {
        "release_manifest_path": release_foundation.get("release_manifest_path"),
        "published_sbom": release_foundation.get("published_sbom"),
        "published_attestation": release_foundation.get("published_attestation"),
        "update_manifest_path": release_operations.get("update_manifest_path"),
        "upgrade_support_report": release_operations.get("upgrade_support_report"),
        "channel_catalog": release_operations.get("channel_catalog"),
        "trust_report_json": distribution.get("trust_report_json"),
    }

    dashboard_payload = {
        "contract_id": "objc3c.full_envelope.claimability.dashboard.summary.v1",
        "status": "PASS",
        "current_rollout_class": rollout["current_rollout_class"],
        "production_strength_claimable": rollout["production_strength_claimable"],
        "triggered_reasons": rollout["triggered_reasons"],
        "source_summaries": list(source_summaries.keys()),
        "source_reports": list(integration_reports.keys()),
        "public_claim_class": public_claim_class,
        "support_row_counts_by_class": support_matrix["support_row_counts_by_class"],
        "triggered_release_blockers": release_blockers["triggered_blockers"],
        "dashboard_release_blocker_projection": dashboard_release_blocker_projection,
        "acceptance_matrix": acceptance_matrix,
        "release_artifacts": release_artifacts,
    }

    for required_key in schema.get("required", []):
        expect(required_key in dashboard_payload, f"dashboard payload missing schema-required key: {required_key}")
    for required_key in contract["required_dashboard_fields"]:
        expect(required_key in dashboard_payload, f"dashboard payload missing contract-required key: {required_key}")
    for required_key in dashboard_release_blocker_projection["required_dashboard_fields"]:
        expect(required_key in dashboard_payload, f"dashboard payload missing release-blocker-required key: {required_key}")
    expect(
        set(DASHBOARD_DECISION_FIELDS).issubset(dashboard_payload),
        "dashboard payload missing source-owned decision fields",
    )
    expect(
        set(contract["required_release_artifact_fields"]) == set(release_artifacts),
        "dashboard release artifact field contract drifted",
    )

    public_payload = {
        "contract_id": "objc3c.full_envelope.claimability.public.summary.v1",
        "status": "PASS",
        "current_rollout_class": dashboard_payload["current_rollout_class"],
        "public_claim_class": public_claim_class,
        "production_strength_claimable": dashboard_payload["production_strength_claimable"],
        "triggered_reason_count": len(dashboard_payload["triggered_reasons"]),
        "dashboard_release_blocker": dashboard_release_blocker_projection["blocker"],
        "dashboard_blocks_production_strength_claim": dashboard_release_blocker_projection[
            "blocks_production_strength_claim"
        ],
        "report_markdown_path": repo_rel(REPORT_MD),
    }
    for required_key in contract["required_public_summary_fields"]:
        expect(required_key in public_payload, f"public summary missing contract-required key: {required_key}")
    for required_key in dashboard_release_blocker_projection["required_public_summary_fields"]:
        expect(required_key in public_payload, f"public summary missing release-blocker-required key: {required_key}")
    expect(
        set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(public_payload),
        "public summary missing source-owned decision fields",
    )

    report_text = (
        "# objc3c Full-Envelope Claimability Report\n\n"
        f"- Rollout class: `{dashboard_payload['current_rollout_class']}`\n"
        f"- Public claim class: `{public_claim_class}`\n"
        f"- Production-strength claimable: `{dashboard_payload['production_strength_claimable']}`\n"
        f"- Dashboard release blocker: `{dashboard_release_blocker_projection['blocker']}`\n"
        f"- Dashboard blocks production-strength claim: `{dashboard_release_blocker_projection['blocks_production_strength_claim']}`\n"
        f"- Triggered release blockers: `{len(dashboard_payload['triggered_release_blockers'])}`\n"
        f"- Triggered reasons: `{len(dashboard_payload['triggered_reasons'])}`\n\n"
        "## Acceptance Matrix\n\n"
        + "\n".join(
            f"- `{row['family']}`: status `{row['status']}`, signal `{row['claim_signal']}`"
            for row in acceptance_matrix
        )
        + "\n"
    )

    OUT_REPORT_DIR.mkdir(parents=True, exist_ok=True)
    OUT_ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(DASHBOARD_JSON, dashboard_payload)
    write_json_file(PUBLIC_JSON, public_payload)
    REPORT_MD.write_text(report_text, encoding="utf-8")

    summary = {
        "contract_id": "objc3c.full_envelope.claimability.dashboard.publication.summary.v1",
        "status": "PASS",
        "runner_path": "scripts/build_full_envelope_claimability_dashboard.py",
        "runbook_mentions_dashboard_surface": "## Dashboard And Claim Publication Surface" in runbook_text,
        "dashboard_json": repo_rel(DASHBOARD_JSON),
        "public_summary_json": repo_rel(PUBLIC_JSON),
        "report_markdown_path": repo_rel(REPORT_MD),
        "current_rollout_class": dashboard_payload["current_rollout_class"],
        "public_claim_class": public_claim_class,
    }
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
