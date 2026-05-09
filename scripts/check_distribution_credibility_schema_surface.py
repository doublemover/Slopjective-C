#!/usr/bin/env python3
"""Validate the checked-in distribution-credibility schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"

EXPECTED_SCHEMAS = (
    (
        "dashboard_schema",
        "objc3c-distribution-credibility-dashboard-v1",
        "https://objc3c.dev/schemas/objc3c-distribution-credibility-dashboard-v1.schema.json",
        "objc3c.distribution.credibility.dashboard.summary.v1",
    ),
    (
        "trust_report_schema",
        "objc3c-distribution-trust-report-v1",
        "https://objc3c.dev/schemas/objc3c-distribution-trust-report-v1.schema.json",
        "objc3c.distribution.trust.report.v1",
    ),
)


def fail(message: str) -> int:
    print(f"distribution-credibility-schema-surface: {message}", file=sys.stderr)
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
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")

    schema_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        surface_key,
        schema_id,
        expected_schema_id,
        expected_contract_id,
    ) in EXPECTED_SCHEMAS:
        raw_path = surface.get(surface_key)
        expected_path = repo_rel(schema_path(schema_id))
        if raw_path != expected_path:
            return fail(f"{surface_key} drifted from registered schema path {expected_path}")
        payload = load_schema(schema_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(f"{expected_path} drifted from expected schema id {expected_schema_id}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{surface_key} contract identity drifted")

        schema_paths.append(expected_path)
        schema_ids.append(expected_schema_id)
        schema_refs[surface_key] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "dashboard_schema": schema_refs["dashboard_schema"],
        "trust_report_schema": schema_refs["trust_report_schema"],
        "schemas": schema_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("distribution-credibility-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
