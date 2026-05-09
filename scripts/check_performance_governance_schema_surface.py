#!/usr/bin/env python3
"""Validate the checked-in performance-governance schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"
EXPECTED_SCHEMAS = (
    (
        "dashboard_summary_schema",
        "objc3c-performance-dashboard-summary-v1",
        "https://objc3c.dev/schemas/objc3c-performance-dashboard-summary-v1.schema.json",
        "objc3c.performance.governance.dashboard.summary.v1",
    ),
    (
        "public_report_schema",
        "objc3c-performance-public-report-v1",
        "https://objc3c.dev/schemas/objc3c-performance-public-report-v1.schema.json",
        "objc3c.performance.governance.public.summary.v1",
    ),
)



def fail(message: str) -> int:
    print(f"performance-governance-schema-surface: FAIL\n- {message}", file=sys.stderr)
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
    if surface.get("contract_id") != "objc3c.performance.governance.schema.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_performance_governance_schema_surface.py":
        return fail("schema_check_script drifted")

    checked_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        surface_key,
        registry_id,
        expected_schema_url,
        expected_contract_id,
    ) in EXPECTED_SCHEMAS:
        expected_path = repo_rel(schema_path(registry_id))
        if surface.get(surface_key) != expected_path:
            return fail(f"{surface_key} drifted from registered schema path {expected_path}")

        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_url:
            return fail(f"{expected_path} drifted from expected schema id {expected_schema_url}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{surface_key} contract identity drifted")

        checked_paths.append(expected_path)
        schema_ids.append(expected_schema_url)
        schema_refs[surface_key] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "dashboard_summary_schema": schema_refs["dashboard_summary_schema"],
        "public_report_schema": schema_refs["public_report_schema"],
        "schemas": checked_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("performance-governance-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
