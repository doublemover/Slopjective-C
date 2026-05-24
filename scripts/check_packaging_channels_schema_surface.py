#!/usr/bin/env python3
"""Validate the checked-in objc3c packaging-channels schema surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.packaging.channels.schema.surface.summary.v1"
JSON_SCHEMA_DRAFT = "https://json-schema.org/draft/2020-12/schema"

EXPECTED_SCHEMAS = (
    (
        "package_channels_manifest",
        "objc3c-package-channels-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-package-channels-manifest-v1.schema.json",
        "objc3c.packaging.channels.summary.v1",
        "required_manifest_fields",
    ),
    (
        "install_receipt",
        "objc3c-package-install-receipt-v1",
        "https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
        "objc3c.packaging.channels.install-receipt.v1",
        "required_receipt_fields",
    ),
    (
        "sanitizer_runtime_library_manifest",
        "objc3c-sanitizer-runtime-library-manifest-v1",
        "https://objc3c.dev/schemas/objc3c-sanitizer-runtime-library-manifest-v1.schema.json",
        "objc3c.sanitizer.runtime-library-manifest.v1",
        "required_runtime_library_manifest_fields",
    ),
)


def fail(message: str) -> int:
    print(f"packaging-channels-schema-surface: {message}", file=sys.stderr)
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
    if metadata_surface.get("contract_id") != "objc3c.packaging.channels.metadata.surface.v1":
        return fail("unexpected metadata surface contract_id")
    schema_surface = load_json(SCHEMA_SURFACE)
    if schema_surface.get("contract_id") != "objc3c.packaging.channels.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    schemas = schema_surface.get("schemas")
    if not isinstance(schemas, list) or not schemas:
        return fail("schema surface did not publish schemas")

    expected_paths = [
        repo_rel(schema_path(registry_id))
        for _, registry_id, _, _, _ in EXPECTED_SCHEMAS
    ]
    if schemas != expected_paths:
        return fail(f"schema surface drifted from registered schemas: {schemas}")

    schema_ids: list[str] = []
    schema_refs: dict[str, str] = {}
    for (
        metadata_key,
        registry_id,
        expected_schema_id,
        expected_contract_id,
        required_fields_key,
    ) in EXPECTED_SCHEMAS:
        expected_path = repo_rel(schema_path(registry_id))
        raw_path = metadata_surface.get(metadata_key)
        if raw_path != expected_path:
            return fail(f"metadata surface drifted from registered schema path for {metadata_key}")
        schema_payload = load_schema(registry_id)
        if schema_payload.get("$schema") != JSON_SCHEMA_DRAFT:
            return fail(f"{expected_path} drifted from draft 2020-12")
        if schema_payload.get("$id") != expected_schema_id:
            return fail(f"{expected_path} drifted from expected schema id {expected_schema_id}")
        if property_const(schema_payload, "contract_id") != expected_contract_id:
            return fail(f"{metadata_key} contract identity drifted")
        if not schema_declares_required_fields(schema_payload, metadata_surface.get(required_fields_key)):
            return fail(f"{metadata_key} schema drifted from {required_fields_key}")

        schema_ids.append(expected_schema_id)
        schema_refs[metadata_key] = expected_path

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "metadata_surface": repo_rel(METADATA_SURFACE),
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "package_channels_manifest": schema_refs["package_channels_manifest"],
        "install_receipt": schema_refs["install_receipt"],
        "sanitizer_runtime_library_manifest": schema_refs["sanitizer_runtime_library_manifest"],
        "schema_count": len(schemas),
        "schemas": expected_paths,
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("packaging-channels-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
