#!/usr/bin/env python3
"""Validate the checked-in release-operations schema surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_operations" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-operations" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "schemas/objc3c-update-manifest-v1.schema.json",
    "schemas/objc3c-compatibility-report-v1.schema.json",
}


def fail(message: str) -> int:
    print(f"release-operations-schema-surface: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


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
    if set(schemas) != EXPECTED_SCHEMAS:
        return fail(f"schema set drifted: {schemas}")
    for raw_path in schemas:
        target = ROOT / raw_path
        if not target.is_file():
            return fail(f"missing schema {raw_path}")
        payload = load_json(target)
        if payload.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
            return fail(f"{raw_path} drifted from draft 2020-12")
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.operations.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "schemas": sorted(schemas),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-operations-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
