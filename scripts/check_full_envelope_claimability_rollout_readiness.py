#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/stability_rollout_implementation_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/rollout-readiness"
JSON_OUT = OUT_DIR / "rollout_readiness_summary.json"
MD_OUT = OUT_DIR / "rollout_readiness_summary.md"


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
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    policy_summaries = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_policy_summaries"]
    }
    integration_reports = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_integration_reports"]
    }

    support_matrix = policy_summaries["tmp/reports/full-envelope-claimability/support-matrix/support_matrix_summary.json"]
    claim_policy = policy_summaries["tmp/reports/full-envelope-claimability/claim-policy/claim_policy_summary.json"]
    blocker_summary = policy_summaries["tmp/reports/full-envelope-claimability/release-blockers/release_blocker_summary.json"]
    public_conformance = integration_reports["tmp/reports/public-conformance/integration-summary.json"]
    performance = integration_reports["tmp/reports/performance-governance/integration-summary.json"]
    release_foundation = integration_reports["tmp/reports/release-foundation/integration-summary.json"]
    release_operations = integration_reports["tmp/reports/release-operations/integration-summary.json"]
    distribution = integration_reports["tmp/reports/distribution-credibility/integration-summary.json"]

    triggered_reasons: list[str] = []
    if public_conformance.get("public_status") != "claim-ready":
        triggered_reasons.append(f"public-conformance:{public_conformance.get('public_status')}")
    if performance.get("claim_ready") is not True:
        triggered_reasons.append(f"performance-claim-ready:{performance.get('claim_ready')}")
    if performance.get("release_status") != "release-ready":
        triggered_reasons.append(f"performance-release-status:{performance.get('release_status')}")
    if distribution.get("trust_state") != "ready":
        triggered_reasons.append(f"distribution-trust:{distribution.get('trust_state')}")

    current_rollout_class = "stable"
    if blocker_summary.get("current_rollout_class") == "preview":
        current_rollout_class = "preview"
    elif triggered_reasons:
        current_rollout_class = "candidate"

    production_strength_claimable = (
        current_rollout_class == "stable"
        and public_conformance.get("public_status") == "claim-ready"
        and performance.get("claim_ready") is True
        and performance.get("release_status") == "release-ready"
        and distribution.get("trust_state") == "ready"
    )

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_full_envelope_claimability_rollout_readiness.py",
        "runbook_mentions_rollout_implementation": "## Stability Regression And Rollout Implementation" in runbook_text,
        "all_policy_summaries_pass": all(summary.get("status") == "PASS" for summary in policy_summaries.values()),
        "all_integration_reports_pass": all(report.get("status") == "PASS" for report in integration_reports.values()),
        "derived_rollout_class_is_allowed": current_rollout_class in contract["expected_rollout_classes"],
        "preview_rollout_matches_blocker_state": blocker_summary.get("current_rollout_class") == "preview" and current_rollout_class == "preview",
        "production_strength_claim_is_currently_false": production_strength_claimable is False,
        "release_artifact_paths_remain_published": all(
            bool(value)
            for value in (
                release_foundation.get("release_manifest_path"),
                release_foundation.get("published_sbom"),
                release_foundation.get("published_attestation"),
                release_operations.get("update_manifest_path"),
                release_operations.get("compatibility_report"),
                release_operations.get("channel_catalog"),
                distribution.get("trust_report_json"),
            )
        ),
        "support_matrix_reports_supported_surfaces": support_matrix.get("support_row_counts_by_class", {}).get("supported", 0) > 0,
        "claim_policy_keeps_preview_non_production": claim_policy.get("checks", {}).get("preview_window_excludes_supported_claims") is True,
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.stability.rollout.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_full_envelope_claimability_rollout_readiness.py",
        "current_rollout_class": current_rollout_class,
        "production_strength_claimable": production_strength_claimable,
        "triggered_reasons": triggered_reasons,
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Rollout Readiness Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Current rollout class: `{payload['current_rollout_class']}`\n"
        f"- Production-strength claimable: `{payload['production_strength_claimable']}`\n"
        f"- Triggered reasons: `{len(payload['triggered_reasons'])}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
