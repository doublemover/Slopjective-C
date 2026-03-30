#!/usr/bin/env python3
"""Validate the checked-in performance-governance schema surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "schema_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "schema-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.schema.surface.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def fail(message: str) -> int:
    print(f"performance-governance-schema-surface: FAIL\n- {message}", file=sys.stderr)
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
    if surface.get("contract_id") != "objc3c.performance.governance.schema.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("schema_check_script") != "scripts/check_performance_governance_schema_surface.py":
        return fail("schema_check_script drifted")

    dashboard_schema = surface.get("dashboard_summary_schema")
    public_schema = surface.get("public_report_schema")
    if dashboard_schema != "schemas/objc3c-performance-dashboard-summary-v1.schema.json":
        return fail("dashboard_summary_schema drifted")
    if public_schema != "schemas/objc3c-performance-public-report-v1.schema.json":
        return fail("public_report_schema drifted")

    dashboard_payload = load_json(require_path(dashboard_schema, kind="dashboard summary schema"))
    public_payload = load_json(require_path(public_schema, kind="public report schema"))

    if dashboard_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.performance.governance.dashboard.summary.v1":
        return fail("dashboard summary schema contract identity drifted")
    if public_payload.get("properties", {}).get("contract_id", {}).get("const") != "objc3c.performance.governance.public.summary.v1":
        return fail("public report schema contract identity drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "schema_surface_contract": repo_rel(SCHEMA_SURFACE),
        "dashboard_summary_schema": dashboard_schema,
        "public_report_schema": public_schema,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("performance-governance-schema-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
