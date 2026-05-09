#!/usr/bin/env python3
"""Validate the checked-in release-foundation schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.release.foundation.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"

EXPECTED_SCHEMAS = {
    "release_manifest_schema": (
        "objc3c-release-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-release-manifest-v1.schema.json",
        "objc3c.release.foundation.manifest.v1",
    ),
    "release_sbom_schema": (
        "objc3c-release-sbom-v1",
        "https://objc3c.dev/schemas/objc3c-release-sbom-v1.schema.json",
        "objc3c.release.foundation.sbom.v1",
    ),
    "release_attestation_schema": (
        "objc3c-release-attestation-v1",
        "https://objc3c.dev/schemas/objc3c-release-attestation-v1.schema.json",
        "objc3c.release.foundation.attestation.v1",
    ),
}


def fail(message: str) -> int:
    print(f"release-foundation-schema-surface: {message}", file=sys.stderr)
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
    if surface.get("contract_id") != "objc3c.release.foundation.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("unexpected schema surface version")
    if surface.get("schema_check_script") != "scripts/check_release_foundation_schema_surface.py":
        return fail("schema_check_script drifted")

    schema_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for field_name, (
        registry_id,
        expected_schema_id,
        expected_contract_id,
    ) in EXPECTED_SCHEMAS.items():
        raw_path = surface.get(field_name)
        expected_path = repo_rel(schema_path(registry_id))
        if raw_path != expected_path:
            return fail(f"{field_name} drifted from registered schema path {expected_path}")
        payload = load_schema(registry_id)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_id:
            return fail(f"{field_name} drifted from expected schema id {expected_schema_id}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{field_name} contract identity drifted")

        schema_paths.append(expected_path)
        schema_ids.append(expected_schema_id)
        schema_refs[field_name] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "release_manifest_schema": schema_refs["release_manifest_schema"],
        "release_sbom_schema": schema_refs["release_sbom_schema"],
        "release_attestation_schema": schema_refs["release_attestation_schema"],
        "schemas": schema_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
