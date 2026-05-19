#!/usr/bin/env python3
"""Validate the checked-in governance-sustainability schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "schema-surface" / "governance_schema_surface_summary.json"
SUMMARY_CONTRACT_ID = "objc3c.governance.sustainability.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"
EXPECTED_SCHEMAS = (
    (
        "budget_summary_schema",
        "objc3c-governance-budget-summary-v1",
        "schemas/objc3c-governance-budget-summary-v1.schema.json",
        "objc3c.governance.sustainability.budget.summary.v1",
    ),
    (
        "anti_regression_summary_schema",
        "objc3c-governance-anti-regression-summary-v1",
        "schemas/objc3c-governance-anti-regression-summary-v1.schema.json",
        "objc3c.governance.sustainability.anti_regression.summary.v1",
    ),
    (
        "governance_evidence_schema",
        "objc3c-governance-sustainability-evidence-v1",
        "schemas/objc3c-governance-sustainability-evidence-v1.schema.json",
        "objc3c.governance.sustainability.evidence.v1",
    ),
)


def fail(message: str) -> int:
    print(f"governance-sustainability-schema-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def property_const(schema_payload: dict[str, Any], property_name: str) -> Any:
    properties = schema_payload.get("properties")
    if not isinstance(properties, dict):
        return None
    property_payload = properties.get(property_name)
    if not isinstance(property_payload, dict):
        return None
    return property_payload.get("const")


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

    checked_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        surface_key,
        schema_id,
        expected_schema_id,
        expected_contract_id,
    ) in EXPECTED_SCHEMAS:
        expected_schema = repo_rel(schema_path(schema_id))
        published_schema = surface.get(surface_key)
        if published_schema != expected_schema:
            return fail(f"{surface_key} drifted from registered schema path {expected_schema}")

        payload = load_schema(schema_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_schema} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(f"{expected_schema} drifted from expected schema id {expected_schema_id}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{surface_key} contract identity drifted")

        checked_paths.append(expected_schema)
        schema_ids.append(expected_schema_id)
        schema_refs[surface_key] = expected_schema

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "budget_summary_schema": schema_refs["budget_summary_schema"],
        "anti_regression_summary_schema": schema_refs["anti_regression_summary_schema"],
        "governance_evidence_schema": schema_refs["governance_evidence_schema"],
        "schemas": checked_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("governance-sustainability-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
