#!/usr/bin/env python3
"""Validate the checked-in objc3c packaging-channels schema surface."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "schema-surface-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


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
        schema_path = ROOT / raw_path.replace("/", "\\")
        if not schema_path.is_file():
            raise RuntimeError(f"missing packaging-channels schema {raw_path}")
        schema_payload = load_json(schema_path)
        schema_id = schema_payload.get("$id")
        if not isinstance(schema_id, str) or not schema_id:
            raise RuntimeError(f"schema {raw_path} did not publish $id")
        schema_ids.append(schema_id)

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
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("packaging-channels-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
