#!/usr/bin/env python3
from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
POLICY_PATH = FIXTURE_ROOT / "sustainable_progress_policy.json"
BUDGET_INVENTORY_PATH = FIXTURE_ROOT / "budget_inventory.json"
WAIVER_REGISTRY_PATH = FIXTURE_ROOT / "waiver_registry.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "sustainable-progress-policy"
SUMMARY_PATH = OUT_DIR / "governance_policy_summary.json"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_timestamp(raw: Any) -> datetime | None:
    if not isinstance(raw, str) or not raw:
        return None
    candidate = raw.replace("Z", "+00:00")
    try:
        return datetime.fromisoformat(candidate)
    except ValueError:
        return None


def main() -> int:
    policy = read_json(POLICY_PATH)
    inventory = read_json(BUDGET_INVENTORY_PATH)
    waiver_registry = read_json(WAIVER_REGISTRY_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    governed_surfaces = policy.get("governed_budget_surfaces", [])
    inventory_surface_ids = {
        entry["surface_id"]
        for entry in inventory.get("measured_budget_surfaces", [])
        if isinstance(entry, dict) and "surface_id" in entry
    }
    policy_surface_ids = {
        entry["surface_id"]
        for entry in governed_surfaces
        if isinstance(entry, dict) and "surface_id" in entry
    }
    missing_policy_surfaces = sorted(inventory_surface_ids - policy_surface_ids)
    unknown_policy_surfaces = sorted(policy_surface_ids - inventory_surface_ids)

    waivers = waiver_registry.get("waivers", [])
    failures: list[str] = []
    if not isinstance(waivers, list):
        failures.append("waiver registry waivers field must be a list")
        waivers = []

    exception_requirements = policy.get("exception_requirements", {})
    required_fields = list(exception_requirements.get("required_fields", []))
    allowed_statuses = set(exception_requirements.get("allowed_statuses", []))
    now = datetime.now(timezone.utc)
    active_waiver_counts: dict[str, int] = {}
    expired_waivers: list[str] = []
    invalid_waivers: list[str] = []
    for waiver in waivers:
        if not isinstance(waiver, dict):
            invalid_waivers.append("non-object-waiver")
            continue
        waiver_id = str(waiver.get("waiver_id", "unknown-waiver"))
        missing_fields = [
            field
            for field in required_fields
            if field not in waiver or waiver.get(field) in (None, "", [])
        ]
        if missing_fields:
            invalid_waivers.append(f"{waiver_id}:missing={','.join(missing_fields)}")
        status = str(waiver.get("status", ""))
        if status not in allowed_statuses:
            invalid_waivers.append(f"{waiver_id}:status={status}")
        expiry = parse_timestamp(waiver.get("expires_at_utc"))
        if expiry is None:
            invalid_waivers.append(f"{waiver_id}:invalid-expiry")
        elif expiry <= now and status == "active":
            expired_waivers.append(waiver_id)
        surface_id = str(waiver.get("surface_id", ""))
        if status == "active" and surface_id:
            active_waiver_counts[surface_id] = active_waiver_counts.get(surface_id, 0) + 1

    max_active = int(exception_requirements.get("max_active_waivers_per_surface", 1))
    overfull_surfaces = sorted(
        surface for surface, count in active_waiver_counts.items() if count > max_active
    )

    summary = {
        "issue": "governance-sustainable-progress-policy",
        "contract_id": policy["contract_id"],
        "governed_surface_count": len(governed_surfaces),
        "inventory_surface_count": len(inventory_surface_ids),
        "missing_policy_surfaces": missing_policy_surfaces,
        "unknown_policy_surfaces": unknown_policy_surfaces,
        "waiver_registry_path": policy.get("waiver_registry"),
        "waiver_count": len(waivers),
        "expired_waivers": expired_waivers,
        "invalid_waivers": invalid_waivers,
        "overfull_surfaces": overfull_surfaces,
        "runbook_mentions_policy": "tests/tooling/fixtures/governance_sustainability/sustainable_progress_policy.json" in runbook_text,
        "runbook_mentions_waiver_registry": "tests/tooling/fixtures/governance_sustainability/waiver_registry.json" in runbook_text,
        "runbook_mentions_summary_script": "python scripts/build_governance_policy_summary.py" in runbook_text,
        "claim_statuses": [
            entry.get("status")
            for entry in policy.get("claim_statuses", [])
            if isinstance(entry, dict)
        ],
        "failures": failures,
    }
    summary["ok"] = all(
        [
            not missing_policy_surfaces,
            not unknown_policy_surfaces,
            not expired_waivers,
            not invalid_waivers,
            not overfull_surfaces,
            not failures,
            summary["runbook_mentions_policy"],
            summary["runbook_mentions_waiver_registry"],
            summary["runbook_mentions_summary_script"],
        ]
    )

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
