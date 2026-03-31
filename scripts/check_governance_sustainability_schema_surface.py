#!/usr/bin/env python3
"""Validate the checked-in governance-sustainability schema surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-C001" / "governance_schema_surface_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.governance.sustainability.schema.surface.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def fail(message: str) -> int:
    print(f"governance-sustainability-schema-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface contract: {repo_rel(SCHEMA_SURFACE)}")

    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.governance.sustainability.schema.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_governance_sustainability_schema_surface.py":
        return fail("schema_check_script drifted")

    budget_schema = surface.get("budget_summary_schema")
    anti_regression_schema = surface.get("anti_regression_summary_schema")
    if budget_schema != "schemas/objc3c-governance-budget-summary-v1.schema.json":
        return fail("budget_summary_schema drifted")
    if anti_regression_schema != "schemas/objc3c-governance-anti-regression-summary-v1.schema.json":
        return fail("anti_regression_summary_schema drifted")

    budget_payload = load_json(require_path(budget_schema, kind="budget summary schema"))
    anti_regression_payload = load_json(require_path(anti_regression_schema, kind="anti-regression summary schema"))

    if budget_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.governance.sustainability.budget.summary.v1":
        return fail("budget summary schema contract identity drifted")
    if anti_regression_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.governance.sustainability.anti_regression.summary.v1":
        return fail("anti-regression summary schema contract identity drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "schema_surface_contract": repo_rel(SCHEMA_SURFACE),
        "budget_summary_schema": budget_schema,
        "anti_regression_summary_schema": anti_regression_schema
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("governance-sustainability-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
