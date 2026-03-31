#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

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


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


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

    triggered_blockers: list[dict[str, Any]] = []
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
            triggered_blockers.append(blocker)

    current_rollout_class = "stable"
    if triggered_blockers:
        current_rollout_class = "preview"
    elif reports["public-conformance"]["payload"].get("public_status") == "caution":
        current_rollout_class = "candidate"

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
        "current_state_is_not_stable_rollout_ready": current_rollout_class != "stable",
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.release.blocker.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_full_envelope_claimability_release_blocker_summary.py",
        "runbook": repo_rel(RUNBOOK_PATH),
        "required_report_count": len(contract["required_reports"]),
        "release_blocker_count": len(contract["release_blockers"]),
        "triggered_blocker_count": len(triggered_blockers),
        "current_rollout_class": current_rollout_class,
        "triggered_blockers": [blocker["blocker"] for blocker in triggered_blockers],
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
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
