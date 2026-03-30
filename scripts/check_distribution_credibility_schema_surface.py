#!/usr/bin/env python3
"""Validate the checked-in distribution-credibility schema surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "schema-surface-summary.json"

EXPECTED_SCHEMAS = {
    "schemas/objc3c-distribution-credibility-dashboard-v1.schema.json",
    "schemas/objc3c-distribution-trust-report-v1.schema.json",
}


def fail(message: str) -> int:
    print(f"distribution-credibility-schema-surface: {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{path} did not contain a JSON object")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface {repo_rel(SCHEMA_SURFACE)}")
    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        return fail("unexpected schema surface contract_id")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")

    schema_paths = {surface.get("dashboard_schema"), surface.get("trust_report_schema")}
    if schema_paths != EXPECTED_SCHEMAS:
        return fail(f"schema set drifted: {sorted(str(path) for path in schema_paths)}")
    for raw_path in sorted(schema_paths):
        if not isinstance(raw_path, str):
            return fail("schema path drifted from string contract")
        target = ROOT / raw_path
        if not target.is_file():
            return fail(f"missing schema {raw_path}")
        payload = load_json(target)
        if payload.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
            return fail(f"{raw_path} drifted from draft 2020-12")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": "objc3c.distribution.credibility.schema.surface.summary.v1",
        "status": "PASS",
        "schema_surface": repo_rel(SCHEMA_SURFACE),
        "schemas": sorted(str(path) for path in schema_paths),
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("distribution-credibility-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
