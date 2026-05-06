#!/usr/bin/env python3
"""Validate the checked-in release-operations schema surface."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "objc3c-update-manifest-v1": "https://objc3c.dev/schemas/objc3c-update-manifest-v1.schema.json",
    "objc3c-compatibility-report-v1": "https://objc3c.dev/schemas/objc3c-compatibility-report-v1.schema.json",
}


def fail(message: str) -> int:
    print(f"release-operations-schema-surface: {message}", file=sys.stderr)
    return 1




def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.release.operations.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_release_operations_schema_surface.py":
        return fail("schema_check_script drifted")
    schemas = surface.get("schemas")
    if not isinstance(schemas, list) or not schemas:
        return fail("schemas must be a non-empty list")
    expected_paths = {repo_rel(schema_path(schema_id)) for schema_id in EXPECTED_SCHEMAS}
    if set(schemas) != expected_paths:
        return fail(f"schema set drifted: {schemas}")
    for schema_id, expected_schema_url in EXPECTED_SCHEMAS.items():
        raw_path = repo_rel(schema_path(schema_id))
        payload = load_schema(schema_id)
        if payload.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
            return fail(f"{raw_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_url:
            return fail(f"{raw_path} drifted from expected schema id {expected_schema_url}")
    summary = {
        "contract_id": "objc3c.release.operations.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "schemas": sorted(schemas),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-operations-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
