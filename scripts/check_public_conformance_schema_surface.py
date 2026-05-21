#!/usr/bin/env python3
"""Validate the checked-in public conformance schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "public_conformance_reporting"
    / "schema_surface.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "public-conformance" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"
EXPECTED_SCHEMAS = (
    (
        "dashboard_status_schema",
        "objc3-conformance-dashboard-status-v1",
        "https://objc3c.dev/schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "schema_id",
        "objc3-conformance-dashboard-status/v1",
    ),
    (
        "public_suite_schema",
        "objc3c-public-conformance-suite-v1",
        "https://objc3c.dev/schemas/objc3c-public-conformance-suite-v1.schema.json",
        "contract_id",
        "objc3c.public_conformance_suite.manifest.v1",
    ),
    (
        "public_scorecard_schema",
        "objc3c-public-conformance-scorecard-v1",
        "https://objc3c.dev/schemas/objc3c-public-conformance-scorecard-v1.schema.json",
        "contract_id",
        "objc3c.public_conformance_reporting.scorecard.summary.v1",
    ),
    (
        "public_summary_schema",
        "objc3c-public-conformance-summary-v1",
        "https://objc3c.dev/schemas/objc3c-public-conformance-summary-v1.schema.json",
        "contract_id",
        "objc3c.public_conformance_reporting.summary.v1",
    ),
)


def fail(message: str) -> int:
    print(f"public-conformance-schema-surface: FAIL\n- {message}", file=sys.stderr)
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
    if surface.get("contract_id") != "objc3c.public_conformance_reporting.schema.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_public_conformance_schema_surface.py":
        return fail("schema_check_script drifted")

    checked_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        surface_key,
        registry_id,
        expected_schema_url,
        identity_property,
        expected_identity,
    ) in EXPECTED_SCHEMAS:
        expected_path = repo_rel(schema_path(registry_id))
        if surface.get(surface_key) != expected_path:
            return fail(f"{surface_key} drifted from registered schema path {expected_path}")

        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_url:
            return fail(f"{expected_path} drifted from expected schema id {expected_schema_url}")
        if property_const(payload, identity_property) != expected_identity:
            return fail(f"{surface_key} contract identity drifted")

        checked_paths.append(expected_path)
        schema_ids.append(expected_schema_url)
        schema_refs[surface_key] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "dashboard_status_schema": schema_refs["dashboard_status_schema"],
        "public_suite_schema": schema_refs["public_suite_schema"],
        "public_scorecard_schema": schema_refs["public_scorecard_schema"],
        "public_summary_schema": schema_refs["public_summary_schema"],
        "schemas": checked_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("public-conformance-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

