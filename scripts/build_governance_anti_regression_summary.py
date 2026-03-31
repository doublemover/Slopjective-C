#!/usr/bin/env python3
"""Build the long-horizon governance anti-regression summary."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
FIXTURE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "anti_regression_reporting_contract.json"
OUT_DIR = ROOT / "tmp" / "reports" / "m318" / "M318-D002"
SUMMARY_PATH = OUT_DIR / "governance_anti_regression_summary.json"
HISTORY_PATH = ROOT / "tmp" / "artifacts" / "governance-sustainability" / "anti-regression-history.json"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_optional_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    return read_json(path)


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def metric_value(metric_id: str, inventory: dict[str, Any], enforcement: dict[str, Any]) -> int:
    measured = inventory.get("measured", {})
    enforcement_measured = enforcement.get("measured", {})
    if metric_id == "package_script_count":
        return int(measured.get("package_script_count", 0))
    if metric_id == "public_workflow_action_count":
        return int(measured.get("public_workflow_action_count", 0))
    if metric_id == "runbook_count":
        return int(measured.get("runbook_count", 0))
    if metric_id == "schema_count":
        return int(measured.get("schema_count", 0))
    if metric_id == "live_check_script_count":
        return int(measured.get("live_check_script_count", 0))
    if metric_id == "active_waiver_count":
        return int(enforcement.get("waiver_status_summary", {}).get("active_waiver_count", 0))
    if metric_id == "failure_count":
        return len(enforcement.get("failures", []))
    raise KeyError(metric_id)


def main() -> int:
    contract = read_json(FIXTURE_PATH)
    inventory = load_optional_json(ROOT / contract["source_summaries"][0]) or {}
    policy = load_optional_json(ROOT / contract["source_summaries"][1]) or {}
    review = load_optional_json(ROOT / contract["source_summaries"][2]) or {}
    enforcement = load_optional_json(ROOT / contract["source_summaries"][3]) or {}

    history_payload = load_optional_json(HISTORY_PATH) or {
        "contract_id": "objc3c.governance.sustainability.anti_regression.history.v1",
        "snapshots": []
    }
    snapshots = history_payload.get("snapshots", [])
    if not isinstance(snapshots, list):
        snapshots = []

    tracked_metrics = list(contract.get("tracked_metrics", []))
    current_metrics = {metric_id: metric_value(metric_id, inventory, enforcement) for metric_id in tracked_metrics}
    current_snapshot = {
        "snapshot_id": f"m318-d002-{datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ')}",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "metrics": current_metrics,
        "inventory_ok": bool(inventory.get("ok", False)),
        "policy_ok": bool(policy.get("ok", False)),
        "maintainer_review_ok": bool(review.get("ok", False)),
        "budget_enforcement_status": enforcement.get("status", "FAIL")
    }
    snapshots.append(current_snapshot)
    history_payload["snapshots"] = snapshots

    previous_snapshot = snapshots[-2] if len(snapshots) > 1 else None
    trend_lines = []
    for metric_id in tracked_metrics:
        current_value = current_metrics[metric_id]
        previous_value = previous_snapshot["metrics"].get(metric_id) if previous_snapshot else None
        if previous_value is None:
            direction = "baseline"
            delta = 0
        else:
            delta = current_value - int(previous_value)
            direction = "up" if delta > 0 else "down" if delta < 0 else "flat"
        trend_lines.append(
            {
                "metric_id": metric_id,
                "current_value": current_value,
                "previous_value": previous_value,
                "delta": delta,
                "direction": direction
            }
        )

    failures: list[str] = []
    if enforcement.get("status") != "PASS":
        failures.append("budget enforcement is not passing")
    if not inventory.get("ok", False):
        failures.append("inventory summary is not passing")
    if not policy.get("ok", False):
        failures.append("policy summary is not passing")
    if not review.get("ok", False):
        failures.append("maintainer review summary is not passing")

    summary = {
        "contract_id": "objc3c.governance.sustainability.anti_regression.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "history_window": {
            "snapshot_count": len(snapshots),
            "history_path": repo_rel(HISTORY_PATH)
        },
        "current_snapshot": current_snapshot,
        "trend_lines": trend_lines,
        "source_summaries": contract["source_summaries"],
        "failures": failures
    }

    HISTORY_PATH.parent.mkdir(parents=True, exist_ok=True)
    HISTORY_PATH.write_text(json.dumps(history_payload, indent=2) + "\n", encoding="utf-8")
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
