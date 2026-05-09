#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel, resolve_repo_path
try:
    from build_full_envelope_claimability_contracts import (
        CLAIM_POLICY_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PERFORMANCE_GOVERNANCE_SUMMARY,
        PUBLIC_CONFORMANCE_SUMMARY,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROLLOUT_CLASS_STABLE,
        SUPPORT_MATRIX_SUMMARY,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        CLAIM_POLICY_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PERFORMANCE_GOVERNANCE_SUMMARY,
        PUBLIC_CONFORMANCE_SUMMARY,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_BLOCKER_SUMMARY,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROLLOUT_CLASS_STABLE,
        SUPPORT_MATRIX_SUMMARY,
    )

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

    support_matrix = policy_summaries[SUPPORT_MATRIX_SUMMARY]
    claim_policy = policy_summaries[CLAIM_POLICY_SUMMARY]
    blocker_summary = policy_summaries[RELEASE_BLOCKER_SUMMARY]
    public_conformance = integration_reports[PUBLIC_CONFORMANCE_SUMMARY]
    performance = integration_reports[PERFORMANCE_GOVERNANCE_SUMMARY]
    release_foundation = integration_reports[RELEASE_FOUNDATION_SUMMARY]
    release_operations = integration_reports[RELEASE_OPERATIONS_SUMMARY]
    distribution = integration_reports[DISTRIBUTION_CREDIBILITY_SUMMARY]

    triggered_reasons: list[str] = []
    if public_conformance.get("public_status") != "claim-ready":
        triggered_reasons.append(f"public-conformance:{public_conformance.get('public_status')}")
    if performance.get("claim_ready") is not True:
        triggered_reasons.append(f"performance-claim-ready:{performance.get('claim_ready')}")
    if performance.get("release_status") != "release-ready":
        triggered_reasons.append(f"performance-release-status:{performance.get('release_status')}")
    if distribution.get("trust_state") != "ready":
        triggered_reasons.append(f"distribution-trust:{distribution.get('trust_state')}")

    current_rollout_class = ROLLOUT_CLASS_STABLE
    if blocker_summary.get("current_rollout_class") == ROLLOUT_CLASS_PREVIEW:
        current_rollout_class = ROLLOUT_CLASS_PREVIEW
    elif triggered_reasons:
        current_rollout_class = ROLLOUT_CLASS_CANDIDATE

    production_strength_claimable = (
        current_rollout_class == ROLLOUT_CLASS_STABLE
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
        "preview_rollout_matches_blocker_state": blocker_summary.get("current_rollout_class") == ROLLOUT_CLASS_PREVIEW and current_rollout_class == ROLLOUT_CLASS_PREVIEW,
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
        "rollout_contract_uses_source_owned_policy_inputs": contract["required_policy_summaries"] == [
            SUPPORT_MATRIX_SUMMARY,
            CLAIM_POLICY_SUMMARY,
            RELEASE_BLOCKER_SUMMARY,
        ],
        "rollout_contract_uses_source_owned_release_inputs": contract["required_integration_reports"] == [
            PUBLIC_CONFORMANCE_SUMMARY,
            PERFORMANCE_GOVERNANCE_SUMMARY,
            RELEASE_FOUNDATION_SUMMARY,
            RELEASE_OPERATIONS_SUMMARY,
            DISTRIBUTION_CREDIBILITY_SUMMARY,
        ],
        "rollout_artifact_fields_match_owner_constants": set(contract["required_release_artifact_fields"]) == set(RELEASE_ARTIFACT_FIELDS),
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
