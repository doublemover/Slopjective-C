#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json"
SCHEMA_PATH = ROOT / "schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_REPORT_DIR = ROOT / "tmp/reports/full-envelope-claimability"
OUT_ARTIFACT_DIR = ROOT / "tmp/artifacts/full-envelope-claimability/report"
DASHBOARD_JSON = OUT_REPORT_DIR / "dashboard-summary.json"
PUBLIC_JSON = OUT_REPORT_DIR / "public-summary.json"
REPORT_MD = OUT_ARTIFACT_DIR / "full-envelope-claimability-report.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


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

    support_matrix = source_summaries["tmp/reports/full-envelope-claimability/support-matrix/support_matrix_summary.json"]
    claim_policy = source_summaries["tmp/reports/full-envelope-claimability/claim-policy/claim_policy_summary.json"]
    release_blockers = source_summaries["tmp/reports/full-envelope-claimability/release-blockers/release_blocker_summary.json"]
    rollout = source_summaries["tmp/reports/full-envelope-claimability/rollout-readiness/rollout_readiness_summary.json"]

    public_conformance = integration_reports["tmp/reports/public-conformance/integration-summary.json"]
    performance = integration_reports["tmp/reports/performance-governance/integration-summary.json"]
    release_foundation = integration_reports["tmp/reports/release-foundation/integration-summary.json"]
    release_operations = integration_reports["tmp/reports/release-operations/integration-summary.json"]
    distribution = integration_reports["tmp/reports/distribution-credibility/integration-summary.json"]

    public_claim_class = "preview-only"
    if rollout["current_rollout_class"] == "stable" and rollout["production_strength_claimable"]:
        public_claim_class = "production-strength"
    elif rollout["current_rollout_class"] == "candidate":
        public_claim_class = "candidate-scoped"

    acceptance_matrix = [
        {
            "family": "public-conformance",
            "status": public_conformance.get("status"),
            "claim_signal": public_conformance.get("public_status"),
        },
        {
            "family": "performance-governance",
            "status": performance.get("status"),
            "claim_signal": performance.get("release_status"),
        },
        {
            "family": "release-foundation",
            "status": release_foundation.get("status"),
            "claim_signal": "publishable" if release_foundation.get("release_manifest_path") else "missing-artifacts",
        },
        {
            "family": "release-operations",
            "status": release_operations.get("status"),
            "claim_signal": "publishable" if release_operations.get("compatibility_report") else "missing-artifacts",
        },
        {
            "family": "distribution-credibility",
            "status": distribution.get("status"),
            "claim_signal": distribution.get("trust_state"),
        },
    ]

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
        "acceptance_matrix": acceptance_matrix,
        "release_artifacts": {
            "release_manifest_path": release_foundation.get("release_manifest_path"),
            "published_sbom": release_foundation.get("published_sbom"),
            "published_attestation": release_foundation.get("published_attestation"),
            "update_manifest_path": release_operations.get("update_manifest_path"),
            "compatibility_report": release_operations.get("compatibility_report"),
            "channel_catalog": release_operations.get("channel_catalog"),
            "trust_report_json": distribution.get("trust_report_json"),
        },
    }

    for required_key in schema.get("required", []):
        expect(required_key in dashboard_payload, f"dashboard payload missing schema-required key: {required_key}")

    public_payload = {
        "contract_id": "objc3c.full_envelope.claimability.public.summary.v1",
        "status": "PASS",
        "current_rollout_class": dashboard_payload["current_rollout_class"],
        "public_claim_class": public_claim_class,
        "production_strength_claimable": dashboard_payload["production_strength_claimable"],
        "triggered_reason_count": len(dashboard_payload["triggered_reasons"]),
        "report_markdown_path": repo_rel(REPORT_MD),
    }

    report_text = (
        "# objc3c Full-Envelope Claimability Report\n\n"
        f"- Rollout class: `{dashboard_payload['current_rollout_class']}`\n"
        f"- Public claim class: `{public_claim_class}`\n"
        f"- Production-strength claimable: `{dashboard_payload['production_strength_claimable']}`\n"
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
    DASHBOARD_JSON.write_text(json.dumps(dashboard_payload, indent=2) + "\n", encoding="utf-8")
    PUBLIC_JSON.write_text(json.dumps(public_payload, indent=2) + "\n", encoding="utf-8")
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
