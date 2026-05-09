#!/usr/bin/env python3
"""Validate objc3c distribution-credibility artifacts end to end."""

from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.public_runner import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
TRUST_REPORT_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
TRUST_REPORT_MD = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.md"
DASHBOARD_JSON = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "dashboard" / "distribution-credibility-dashboard.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "end-to-end-summary.json"






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    result = run_capture(public_workflow_command("validate-distribution-credibility"))
    if result.returncode != 0:
        raise RuntimeError("validate-distribution-credibility failed")

    trust_report = load_json(TRUST_REPORT_JSON)
    dashboard = load_json(DASHBOARD_JSON)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    expect(TRUST_REPORT_MD.is_file(), "missing markdown trust report")
    expect(artifact_surface.get("trust_report_json") == repo_rel(TRUST_REPORT_JSON), "trust report JSON path drifted")
    expect(artifact_surface.get("trust_report_markdown") == repo_rel(TRUST_REPORT_MD), "trust report markdown path drifted")
    expect(artifact_surface.get("dashboard_artifact") == repo_rel(DASHBOARD_JSON), "dashboard artifact path drifted")
    expect(trust_report.get("status") == "PASS", "trust report JSON did not pass")
    expect(dashboard.get("status") == "PASS", "dashboard JSON did not pass")
    expect(trust_report.get("dashboard_path") == repo_rel(DASHBOARD_JSON), "trust report dashboard path drifted")
    expect(trust_report.get("trust_state") in {"ready", "degraded", "blocked"}, "trust state drifted")
    expect(trust_report.get("trust_signals") == dashboard.get("trust_signals"), "trust signal publication drifted")
    expect(trust_report.get("required_drill_steps") == dashboard.get("required_drill_steps"), "release drill publication drifted")
    expect(trust_report.get("operator_actions") == dashboard.get("operator_actions"), "operator action publication drifted")
    evidence_paths = trust_report.get("evidence_paths", [])
    expect(isinstance(evidence_paths, list) and len(evidence_paths) >= 5, "evidence paths drifted")
    expect(
        any("release-operations" in str(path) for path in evidence_paths)
        and any("package-channels" in str(path) for path in evidence_paths),
        "trust report evidence no longer references release-operations and package-channels outputs",
    )

    summary = {
        "contract_id": "objc3c.distribution.credibility.end-to-end.summary.v1",
        "status": "PASS",
        "dashboard_json": repo_rel(DASHBOARD_JSON),
        "trust_report_json": repo_rel(TRUST_REPORT_JSON),
        "trust_report_markdown": repo_rel(TRUST_REPORT_MD),
        "trust_state": trust_report.get("trust_state"),
        "evidence_path_count": len(evidence_paths),
        "trust_signal_count": len(trust_report.get("trust_signals", [])),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-distribution-credibility-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
