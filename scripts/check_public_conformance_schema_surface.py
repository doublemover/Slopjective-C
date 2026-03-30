#!/usr/bin/env python3
"""Validate the checked-in public conformance schema surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "public_conformance_reporting"
    / "schema_surface.json"
)
SUMMARY_PATH = ROOT / "tmp" / "reports" / "public-conformance" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.schema.surface.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def fail(message: str) -> int:
    print(f"public-conformance-schema-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def main() -> int:
    if not SCHEMA_SURFACE.is_file():
        return fail(f"missing schema surface contract: {repo_rel(SCHEMA_SURFACE)}")

    surface = load_json(SCHEMA_SURFACE)
    if surface.get("contract_id") != "objc3c.public_conformance_reporting.schema.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_public_conformance_schema_surface.py":
        return fail("schema_check_script drifted")

    dashboard_schema = surface.get("dashboard_status_schema")
    scorecard_schema = surface.get("public_scorecard_schema")
    summary_schema = surface.get("public_summary_schema")
    if dashboard_schema != "schemas/objc3-conformance-dashboard-status-v1.schema.json":
        return fail("dashboard_status_schema drifted")
    if scorecard_schema != "schemas/objc3c-public-conformance-scorecard-v1.schema.json":
        return fail("public_scorecard_schema drifted")
    if summary_schema != "schemas/objc3c-public-conformance-summary-v1.schema.json":
        return fail("public_summary_schema drifted")

    dashboard_path = require_path(dashboard_schema, kind="dashboard status schema")
    scorecard_path = require_path(scorecard_schema, kind="public scorecard schema")
    summary_path = require_path(summary_schema, kind="public summary schema")

    dashboard_payload = load_json(dashboard_path)
    scorecard_payload = load_json(scorecard_path)
    summary_payload = load_json(summary_path)

    if dashboard_payload.get("properties", {}).get("schema_id", {}).get("const") != "objc3-conformance-dashboard-status/v1":
        return fail("dashboard schema identity drifted")
    if scorecard_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.public_conformance_reporting.scorecard.summary.v1":
        return fail("scorecard schema contract identity drifted")
    if summary_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.public_conformance_reporting.summary.v1":
        return fail("summary schema contract identity drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "schema_surface_contract": repo_rel(SCHEMA_SURFACE),
        "dashboard_status_schema": dashboard_schema,
        "public_scorecard_schema": scorecard_schema,
        "public_summary_schema": summary_schema,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("public-conformance-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

