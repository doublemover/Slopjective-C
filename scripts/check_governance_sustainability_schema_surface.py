#!/usr/bin/env python3
"""Validate the checked-in governance-sustainability schema surface."""

from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "schema-surface" / "governance_schema_surface_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.governance.sustainability.schema.surface.summary.v1"
GOVERNANCE_SCHEMA_CONTRACTS = {
    "budget_summary_schema": (
        "objc3c-governance-budget-summary-v1",
        "objc3c.governance.sustainability.budget.summary.v1",
        "budget_summary_schema drifted",
        "budget summary schema contract identity drifted",
    ),
    "anti_regression_summary_schema": (
        "objc3c-governance-anti-regression-summary-v1",
        "objc3c.governance.sustainability.anti_regression.summary.v1",
        "anti_regression_summary_schema drifted",
        "anti-regression summary schema contract identity drifted",
    ),
    "governance_evidence_schema": (
        "objc3c-governance-sustainability-evidence-v1",
        "objc3c.governance.sustainability.evidence.v1",
        "governance_evidence_schema drifted",
        "governance evidence schema contract identity drifted",
    ),
}


def fail(message: str) -> int:
    print(f"governance-sustainability-schema-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


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

    schema_refs: dict[str, str] = {}
    for surface_key, (
        schema_id,
        contract_id,
        path_drift_message,
        contract_drift_message,
    ) in GOVERNANCE_SCHEMA_CONTRACTS.items():
        expected_schema = repo_rel(schema_path(schema_id))
        published_schema = surface.get(surface_key)
        if published_schema != expected_schema:
            return fail(path_drift_message)

        payload = load_schema(schema_id)
        if payload.get("properties", {}).get("contract_id", {}).get("const") != contract_id:
            return fail(contract_drift_message)
        schema_refs[surface_key] = expected_schema

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "schema_surface_contract": repo_rel(SCHEMA_SURFACE),
        "budget_summary_schema": schema_refs["budget_summary_schema"],
        "anti_regression_summary_schema": schema_refs["anti_regression_summary_schema"],
        "governance_evidence_schema": schema_refs["governance_evidence_schema"],
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("governance-sustainability-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
