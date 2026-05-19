#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel, resolve_repo_path
try:
    from build_full_envelope_claimability_contracts import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROLLOUT_CLASS_STABLE,
        public_claim_class_for_rollout,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        NON_PRODUCTION_PUBLIC_CLAIM_CLASSES,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        ROLLOUT_CLASS_CANDIDATE,
        ROLLOUT_CLASS_PREVIEW,
        ROLLOUT_CLASS_STABLE,
        public_claim_class_for_rollout,
    )

ROOT = Path(__file__).resolve().parents[1]
POLICY_CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/release_blocker_rollout_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/release-blockers"
JSON_OUT = OUT_DIR / "release_blocker_summary.json"
MD_OUT = OUT_DIR / "release_blocker_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload




def build_dashboard_release_blocker_projection(
    contract: dict[str, Any],
    current_rollout_class: str,
    production_strength_claimable: bool,
) -> tuple[dict[str, Any], dict[str, Any] | None]:
    projection_contract = contract.get("dashboard_release_blocker_projection")
    expect(
        isinstance(projection_contract, dict),
        "release blocker policy must define dashboard_release_blocker_projection",
    )
    public_claim_class = public_claim_class_for_rollout(
        current_rollout_class, production_strength_claimable
    )
    blocking_public_claim_classes = set(
        projection_contract["blocking_public_claim_classes"]
    )
    blocking_rollout_classes = set(projection_contract["blocking_rollout_classes"])
    blocks_production_strength_claim = (
        public_claim_class in blocking_public_claim_classes
        or current_rollout_class in blocking_rollout_classes
    )
    projection = {
        "blocker": projection_contract["blocker"],
        "dashboard_summary_path": projection_contract["dashboard_summary_path"],
        "public_summary_path": projection_contract["public_summary_path"],
        "required_dashboard_fields": projection_contract["required_dashboard_fields"],
        "required_public_summary_fields": projection_contract["required_public_summary_fields"],
        "source_owned_decision_fields": projection_contract["source_owned_decision_fields"],
        "current_rollout_class": current_rollout_class,
        "public_claim_class": public_claim_class,
        "production_strength_claimable": production_strength_claimable,
        "blocks_production_strength_claim": blocks_production_strength_claim,
    }
    dashboard_blocker = None
    if blocks_production_strength_claim:
        dashboard_blocker = {
            "blocker": projection_contract["blocker"],
            "source_report": "full-envelope-dashboard-projection",
            "field": "public_claim_class",
            "blocking_values": sorted(blocking_public_claim_classes),
        }
    return projection, dashboard_blocker


def main() -> int:
    contract = read_json(POLICY_CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    reports: dict[str, dict[str, Any]] = {}
    for report in contract["required_reports"]:
        path = resolve_repo_path(report["path"])
        expect(path.is_file(), f"missing required report: {repo_rel(path)}")
        payload = read_json(path)
        reports[report["name"]] = {
            "path": repo_rel(path),
            "payload": payload,
            "contract_matches": payload.get("contract_id") == report["required_contract_id"],
            "passes": payload.get("status") == "PASS",
        }

    triggered_policy_blockers: list[dict[str, Any]] = []
    for blocker in contract["release_blockers"]:
        if "source_reports" in blocker:
            triggered = any(
                not reports[name]["contract_matches"] or not reports[name]["passes"]
                for name in blocker["source_reports"]
            )
        elif "required_fields" in blocker:
            payload = reports[blocker["source_report"]]["payload"]
            triggered = any(not payload.get(field) for field in blocker["required_fields"])
        else:
            payload = reports[blocker["source_report"]]["payload"]
            triggered = payload.get(blocker["field"]) in blocker["blocking_values"]

        if triggered:
            triggered_policy_blockers.append(blocker)

    current_rollout_class = ROLLOUT_CLASS_STABLE
    if triggered_policy_blockers:
        current_rollout_class = ROLLOUT_CLASS_PREVIEW
    elif reports["public-conformance"]["payload"].get("public_status") == "caution":
        current_rollout_class = ROLLOUT_CLASS_CANDIDATE
    production_strength_claimable = current_rollout_class == ROLLOUT_CLASS_STABLE
    dashboard_projection, dashboard_blocker = build_dashboard_release_blocker_projection(
        contract, current_rollout_class, production_strength_claimable
    )
    triggered_blockers = list(triggered_policy_blockers)
    if dashboard_blocker is not None:
        triggered_blockers.append(dashboard_blocker)

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_full_envelope_claimability_release_blocker_summary.py",
        "runbook_mentions_release_blocker_criteria": "## Release-Blocker And Rollout Criteria" in runbook_text,
        "runbook_mentions_current_envelope_can_remain_blocked": "current envelope remains\nrelease-blocked" in runbook_text,
        "all_required_reports_match_expected_contracts": all(
            item["contract_matches"] for item in reports.values()
        ),
        "all_required_reports_pass": all(item["passes"] for item in reports.values()),
        "stable_rollout_disallows_release_blocking": any(
            rollout["class"] == "stable" and rollout["allows_release_blocking"] is False
            for rollout in contract["rollout_classes"]
        ),
        "preview_rollout_allows_release_blocking": any(
            rollout["class"] == "preview" and rollout["allows_release_blocking"] is True
            for rollout in contract["rollout_classes"]
        ),
        "current_state_triggers_at_least_one_release_blocker": len(triggered_blockers) > 0,
        "current_state_is_not_stable_rollout_ready": current_rollout_class != ROLLOUT_CLASS_STABLE,
        "dashboard_projection_configured": bool(dashboard_projection["blocker"]),
        "dashboard_projection_names_canonical_outputs": (
            dashboard_projection["dashboard_summary_path"] == DASHBOARD_SUMMARY
            and dashboard_projection["public_summary_path"] == PUBLIC_SUMMARY
        ),
        "dashboard_projection_requires_source_owned_decisions": set(DASHBOARD_DECISION_FIELDS).issubset(
            dashboard_projection["required_dashboard_fields"]
        )
        and set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(
            dashboard_projection["required_public_summary_fields"]
        )
        and set(DASHBOARD_DECISION_FIELDS).issubset(
            dashboard_projection["source_owned_decision_fields"]
        )
        and set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(
            dashboard_projection["source_owned_decision_fields"]
        ),
        "dashboard_projection_blocks_non_production_public_claims": (
            dashboard_projection["blocks_production_strength_claim"]
            == (dashboard_projection["public_claim_class"] in NON_PRODUCTION_PUBLIC_CLAIM_CLASSES)
        ),
        "triggered_blockers_include_dashboard_projection": (
            not dashboard_projection["blocks_production_strength_claim"]
            or dashboard_projection["blocker"]
            in {blocker["blocker"] for blocker in triggered_blockers}
        ),
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.release.blocker.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_full_envelope_claimability_release_blocker_summary.py",
        "runbook": repo_rel(RUNBOOK_PATH),
        "required_report_count": len(contract["required_reports"]),
        "release_blocker_count": len(contract["release_blockers"]),
        "policy_triggered_blocker_count": len(triggered_policy_blockers),
        "dashboard_triggered_blocker_count": 1 if dashboard_blocker else 0,
        "triggered_blocker_count": len(triggered_blockers),
        "current_rollout_class": current_rollout_class,
        "production_strength_claimable": production_strength_claimable,
        "triggered_blockers": [blocker["blocker"] for blocker in triggered_blockers],
        "dashboard_release_blocker_projection": dashboard_projection,
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, payload)
    MD_OUT.write_text(
        "# Full-Envelope Release Blocker Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Required reports: `{payload['required_report_count']}`\n"
        f"- Release blockers: `{payload['release_blocker_count']}`\n"
        f"- Triggered blockers: `{payload['triggered_blocker_count']}`\n"
        f"- Current rollout class: `{payload['current_rollout_class']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
