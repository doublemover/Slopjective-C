#!/usr/bin/env python3
"""Validate the checked-in release-foundation schema surface."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "release_manifest_schema": (
        "objc3c-release-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-release-manifest-v1.schema.json",
    ),
    "release_sbom_schema": (
        "objc3c-release-sbom-v1",
        "https://objc3c.dev/schemas/objc3c-release-sbom-v1.schema.json",
    ),
    "release_attestation_schema": (
        "objc3c-release-attestation-v1",
        "https://objc3c.dev/schemas/objc3c-release-attestation-v1.schema.json",
    ),
}


def fail(message: str) -> int:
    print(f"release-foundation-schema-surface: {message}", file=sys.stderr)
    return 1




def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.release.foundation.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("unexpected schema surface version")
    if surface.get("schema_check_script") != "scripts/check_release_foundation_schema_surface.py":
        return fail("schema_check_script drifted")

    checked_paths = [repo_rel(SCHEMA_SURFACE)]
    for field_name, (registry_id, expected_schema_id) in EXPECTED_SCHEMAS.items():
        raw_path = surface.get(field_name)
        expected_path = repo_rel(schema_path(registry_id))
        if raw_path != expected_path:
            return fail(f"{field_name} drifted from registered schema path {expected_path}")
        payload = load_schema(registry_id)
        if payload.get("$id") != expected_schema_id:
            return fail(f"{field_name} drifted from expected schema id {expected_schema_id}")
        checked_paths.append(expected_path)

    summary = {
        "contract_id": "objc3c.release.foundation.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "checked_paths": checked_paths,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
