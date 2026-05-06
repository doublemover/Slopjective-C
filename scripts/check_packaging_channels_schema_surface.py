#!/usr/bin/env python3
"""Validate the checked-in objc3c packaging-channels schema surface."""

from __future__ import annotations

from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_shared.schema_registry import load_schema, schema_path
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "objc3c-package-channels-manifest-v1",
    "objc3c-package-install-receipt-v1",
}


def main() -> int:
    metadata_surface = load_json(METADATA_SURFACE)
    schema_surface = load_json(SCHEMA_SURFACE)
    schemas = schema_surface.get("schemas")
    if not isinstance(schemas, list) or not schemas:
        raise RuntimeError("schema surface did not publish schemas")
    schema_ids: list[str] = []
    for raw_path in schemas:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError("schema surface contained an invalid schema path")
        registry_id = next(
            (
                schema_id
                for schema_id in EXPECTED_SCHEMAS
                if raw_path == repo_rel(schema_path(schema_id))
            ),
            None,
        )
        if registry_id is None:
            raise RuntimeError(f"schema surface contained an unregistered schema path {raw_path}")
        schema_payload = load_schema(registry_id)
        schema_id = schema_payload.get("$id")
        if not isinstance(schema_id, str) or not schema_id:
            raise RuntimeError(f"schema {raw_path} did not publish $id")
        schema_ids.append(schema_id)

    expected_paths = {repo_rel(schema_path(schema_id)) for schema_id in EXPECTED_SCHEMAS}
    if set(schemas) != expected_paths:
        raise RuntimeError(f"schema surface drifted from registered schemas: {schemas}")

    for required_key in ("package_channels_manifest", "install_receipt"):
        raw_path = metadata_surface.get(required_key)
        if not isinstance(raw_path, str) or raw_path not in schemas:
            raise RuntimeError(f"metadata surface drifted from schema surface for {required_key}")

    summary = {
        "contract_id": "objc3c.packaging.channels.schema.surface.summary.v1",
        "status": "PASS",
        "metadata_surface": repo_rel(METADATA_SURFACE),
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "schema_count": len(schemas),
        "schema_ids": schema_ids,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("packaging-channels-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
