#!/usr/bin/env python3
"""Validate the checked-in release-operations schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.release.operations.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"

EXPECTED_SCHEMAS = (
    (
        "update_manifest",
        "schemas/objc3c-update-manifest-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-update-manifest-v1.schema.json",
        "objc3c.release.operations.update-manifest.v1",
        "required_update_manifest_fields",
    ),
    (
        "release_channel_manifest",
        "schemas/objc3c-release-channel-operations-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-release-channel-operations-v1.schema.json",
        "objc3c.release.operations.channel-manifest.v1",
        None,
    ),
    (
        "upgrade_support_report",
        "schemas/objc3c-upgrade-support-report-v1.schema.json",
        "https://objc3c.dev/schemas/objc3c-upgrade-support-report-v1.schema.json",
        "objc3c.release.operations.upgrade-support-report.v1",
        "required_upgrade_support_report_fields",
    ),
)


def fail(message: str) -> int:
    print(f"release-operations-schema-surface: {message}", file=sys.stderr)
    return 1


def property_const(schema_payload: dict[str, Any], property_name: str) -> Any:
    properties = schema_payload.get("properties")
    if not isinstance(properties, dict):
        return None
    property_payload = properties.get(property_name)
    if not isinstance(property_payload, dict):
        return None
    return property_payload.get("const")


def schema_declares_required_fields(
    schema_payload: dict[str, Any],
    required_fields: object,
) -> bool:
    if not isinstance(required_fields, list) or not required_fields:
        return False
    declared_required = schema_payload.get("required")
    properties = schema_payload.get("properties")
    if not isinstance(declared_required, list) or not isinstance(properties, dict):
        return False
    required_set = set(declared_required)
    return all(
        isinstance(field_name, str)
        and field_name in required_set
        and isinstance(properties.get(field_name), dict)
        for field_name in required_fields
    )


def main() -> int:
    if not METADATA_SURFACE.is_file():
        return fail(f"missing metadata surface {repo_rel(METADATA_SURFACE)}")
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    metadata_surface = load_json(METADATA_SURFACE)
    if metadata_surface.get("contract_id") != "objc3c.release.operations.metadata.surface.v1":
        return fail("unexpected metadata surface contract_id")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.release.operations.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_release_operations_schema_surface.py":
        return fail("schema_check_script drifted")

    schema_paths: list[str] = []
    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        surface_key,
        expected_path,
        expected_schema_url,
        expected_contract_id,
        required_fields_key,
    ) in EXPECTED_SCHEMAS:
        raw_path = surface.get(surface_key)
        if raw_path != expected_path:
            return fail(f"{surface_key} drifted from schema path {expected_path}")
        metadata_path = metadata_surface.get(surface_key)
        if metadata_path != expected_path:
            return fail(f"metadata surface drifted from schema path for {surface_key}")
        payload = load_json(ROOT / expected_path)
        if payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if payload.get("$id") != expected_schema_url:
            return fail(f"{expected_path} drifted from expected schema id {expected_schema_url}")
        if property_const(payload, "contract_id") != expected_contract_id:
            return fail(f"{surface_key} contract identity drifted")
        if required_fields_key is not None and not schema_declares_required_fields(
            payload,
            metadata_surface.get(required_fields_key),
        ):
            return fail(f"{surface_key} schema drifted from {required_fields_key}")

        schema_paths.append(expected_path)
        schema_ids.append(expected_schema_url)
        schema_refs[surface_key] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "metadata_surface": repo_rel(METADATA_SURFACE),
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "update_manifest": schema_refs["update_manifest"],
        "release_channel_manifest": schema_refs["release_channel_manifest"],
        "upgrade_support_report": schema_refs["upgrade_support_report"],
        "schemas": schema_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-operations-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
