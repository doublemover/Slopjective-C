#!/usr/bin/env python3
"""Validate the checked-in performance-governance source surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "performance-governance" / "source-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.source.surface.summary.v1"
EXPECTED_BUILD_SCRIPTS = [
    "scripts/build_objc3c_performance_dashboard.py",
    "scripts/publish_objc3c_performance_report.py",
    "scripts/check_objc3c_performance_governance_integration.py",
    "scripts/check_objc3c_performance_governance_end_to_end.py",
]
EXPECTED_CHECKED_IN_ROOTS = [
    "docs/runbooks",
    "schemas",
    "scripts",
    "tests/tooling/fixtures/performance_governance",
    "tests/tooling/fixtures/performance",
    "tests/tooling/fixtures/compiler_throughput",
    "tests/tooling/fixtures/runtime_performance",
]


def fail(message: str) -> int:
    print(f"performance-governance-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


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
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface contract: {repo_rel(SOURCE_SURFACE)}")

    surface = load_json(SOURCE_SURFACE)
    if surface.get("contract_id") != "objc3c.performance.governance.source.surface.v1":
        return fail("contract_id drifted")
    if surface.get("surface_kind") != "publishable-performance-report-source-surface":
        return fail("surface_kind drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_performance_governance.md":
        return fail("runbook drifted")

    require_path(surface["runbook"], kind="runbook")
    require_path(surface["budget_model"], kind="budget model")
    require_path(surface["claim_policy"], kind="claim policy")
    require_path(surface["breach_triage_policy"], kind="breach triage policy")
    require_path(surface["lab_policy"], kind="lab policy")
    require_path(surface["schema_surface"], kind="schema surface")

    checked_in_sources = surface.get("checked_in_sources")
    if not isinstance(checked_in_sources, list) or len(checked_in_sources) < 4:
        return fail("checked_in_sources drifted")
    for relative_path in checked_in_sources:
        if not isinstance(relative_path, str) or not relative_path:
            return fail("checked_in_sources contains a non-string path")
        require_path(relative_path, kind="checked-in source")

    build_scripts = surface.get("build_scripts")
    if build_scripts != EXPECTED_BUILD_SCRIPTS:
        return fail("build_scripts drifted")
    for relative_path in EXPECTED_BUILD_SCRIPTS:
        require_path(relative_path, kind="build script")

    upstream_reports = surface.get("upstream_reports")
    if not isinstance(upstream_reports, list) or len(upstream_reports) < 4:
        return fail("upstream_reports drifted")

    machine_owned_output_roots = surface.get("machine_owned_output_roots")
    if machine_owned_output_roots != [
        "tmp/reports/performance-governance",
        "tmp/artifacts/performance-governance",
    ]:
        return fail("machine_owned_output_roots drifted")

    explicit_non_goals = surface.get("explicit_non_goals")
    if not isinstance(explicit_non_goals, list) or len(explicit_non_goals) < 3:
        return fail("explicit_non_goals drifted")

    for root in EXPECTED_CHECKED_IN_ROOTS:
        require_path(root, kind="checked-in root")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_contract": repo_rel(SOURCE_SURFACE),
        "runbook": surface["runbook"],
        "checked_in_roots": EXPECTED_CHECKED_IN_ROOTS,
        "checked_in_source_count": len(checked_in_sources),
        "build_scripts": EXPECTED_BUILD_SCRIPTS,
        "upstream_report_paths": upstream_reports,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("performance-governance-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
