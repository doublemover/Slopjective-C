#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel, resolve_repo_path
try:
    from build_full_envelope_claimability_contracts import (
        DASHBOARD_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PUBLIC_SUMMARY,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        DASHBOARD_SUMMARY,
        DISTRIBUTION_CREDIBILITY_SUMMARY,
        PUBLIC_SUMMARY,
        RELEASE_ARTIFACT_FIELDS,
        RELEASE_FOUNDATION_SUMMARY,
        RELEASE_OPERATIONS_SUMMARY,
        ROLLOUT_READINESS_SUMMARY,
    )

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/release_candidate_evidence_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp" / "reports" / "full-envelope-claimability" / "release-candidate-evidence"
JSON_OUT = OUT_DIR / "release_candidate_evidence_summary.json"
MD_OUT = OUT_DIR / "release_candidate_evidence_summary.md"


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
    reports = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_reports"]
    }

    dashboard = reports[DASHBOARD_SUMMARY]
    public_summary = reports[PUBLIC_SUMMARY]
    rollout = reports[ROLLOUT_READINESS_SUMMARY]
    release_foundation = reports[RELEASE_FOUNDATION_SUMMARY]
    release_operations = reports[RELEASE_OPERATIONS_SUMMARY]
    distribution = reports[DISTRIBUTION_CREDIBILITY_SUMMARY]

    artifact_values = {
        "release_manifest_path": release_foundation.get("release_manifest_path"),
        "published_sbom": release_foundation.get("published_sbom"),
        "published_attestation": release_foundation.get("published_attestation"),
        "update_manifest_path": release_operations.get("update_manifest_path"),
        "upgrade_support_report": release_operations.get("upgrade_support_report"),
        "channel_catalog": release_operations.get("channel_catalog"),
        "trust_report_json": distribution.get("trust_report_json"),
    }
    artifact_paths = {
        key: resolve_repo_path(value)
        for key, value in artifact_values.items()
        if isinstance(value, str) and value
    }

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_full_envelope_claimability_release_candidate_evidence.py",
        "runbook_mentions_release_candidate_evidence_section": "## Release-Candidate Evidence Packaging" in runbook_text,
        "all_required_reports_pass": all(report.get("status") == "PASS" for report in reports.values()),
        "all_required_artifact_fields_present": all(bool(artifact_values.get(field)) for field in contract["required_artifact_fields"]),
        "all_required_artifact_paths_exist": all(path.exists() for path in artifact_paths.values()),
        "dashboard_and_public_summary_agree_on_rollout_class": dashboard.get("current_rollout_class") == public_summary.get("current_rollout_class"),
        "dashboard_and_rollout_summary_agree_on_rollout_class": dashboard.get("current_rollout_class") == rollout.get("current_rollout_class"),
        "preview_only_claim_remains_publishable": public_summary.get("public_claim_class") == "preview-only" and dashboard.get("production_strength_claimable") is False,
        "distribution_trust_report_is_published": distribution.get("trust_state") == "ready",
        "required_report_paths_match_owner_constants": contract["required_reports"] == [
            DASHBOARD_SUMMARY,
            PUBLIC_SUMMARY,
            ROLLOUT_READINESS_SUMMARY,
            RELEASE_FOUNDATION_SUMMARY,
            RELEASE_OPERATIONS_SUMMARY,
            DISTRIBUTION_CREDIBILITY_SUMMARY,
        ],
        "required_artifact_fields_match_owner_constants": tuple(contract["required_artifact_fields"]) == RELEASE_ARTIFACT_FIELDS,
        "release_candidate_requires_dashboard_blocker_decision": public_summary.get(
            "dashboard_blocks_production_strength_claim"
        )
        is True,
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.release.candidate.evidence.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_full_envelope_claimability_release_candidate_evidence.py",
        "required_artifact_count": len(contract["required_artifact_fields"]),
        "artifact_paths": {key: repo_rel(path) for key, path in artifact_paths.items()},
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Release Candidate Evidence Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Required artifacts: `{payload['required_artifact_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
