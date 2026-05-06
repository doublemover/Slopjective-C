#!/usr/bin/env python3
"""Validate the checked-in distribution-credibility schema surface."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "dashboard_schema": "objc3c-distribution-credibility-dashboard-v1",
    "trust_report_schema": "objc3c-distribution-trust-report-v1",
}


def fail(message: str) -> int:
    print(f"distribution-credibility-schema-surface: {message}", file=sys.stderr)
    return 1




def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")

    schema_paths: set[str] = set()
    for surface_key, schema_id in EXPECTED_SCHEMAS.items():
        raw_path = surface.get(surface_key)
        expected_path = repo_rel(schema_path(schema_id))
        if raw_path != expected_path:
            return fail(f"{surface_key} drifted from registered schema {expected_path}")
        payload = load_schema(schema_id)
        if payload.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
            return fail(f"{expected_path} drifted from draft 2020-12")
        schema_paths.add(expected_path)

    summary = {
        "contract_id": "objc3c.distribution.credibility.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "schemas": sorted(schema_paths),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("distribution-credibility-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
