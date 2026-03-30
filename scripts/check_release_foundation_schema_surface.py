#!/usr/bin/env python3
"""Validate the checked-in release-foundation schema surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "release_manifest_schema": "https://objc3c.dev/schemas/objc3c-release-manifest-v1.schema.json",
    "release_sbom_schema": "https://objc3c.dev/schemas/objc3c-release-sbom-v1.schema.json",
    "release_attestation_schema": "https://objc3c.dev/schemas/objc3c-release-attestation-v1.schema.json",
}


def fail(message: str) -> int:
    print(f"release-foundation-schema-surface: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace('\\', '/')


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
    for field_name, expected_schema_id in EXPECTED_SCHEMAS.items():
        raw_path = surface.get(field_name)
        if not isinstance(raw_path, str) or not raw_path:
            return fail(f"{field_name} was missing from the schema surface")
        schema_path = ROOT / raw_path
        if not schema_path.is_file():
            return fail(f"{field_name} referenced missing file {raw_path}")
        payload = load_json(schema_path)
        if payload.get("$id") != expected_schema_id:
            return fail(f"{field_name} drifted from expected schema id {expected_schema_id}")
        checked_paths.append(raw_path)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.release.foundation.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "checked_paths": checked_paths,
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
