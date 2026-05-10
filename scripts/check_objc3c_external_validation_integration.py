#!/usr/bin/env python3
"""Validate the integrated external-validation report set and workflow report."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command


ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_SURFACE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "external_validation"
    / "workflow_surface.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "external-validation" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.integration.summary.v1"



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_workflow_surface() -> dict[str, Any]:
    surface = load_json(WORKFLOW_SURFACE)
    expect(
        surface.get("contract_id")
        == "objc3c.external_validation.workflow.surface.v1",
        "external-validation workflow surface contract_id drifted",
    )
    expect(surface.get("schema_version") == 1, "external-validation workflow surface schema_version drifted")
    return surface


def workflow_report_path(validate_action: str) -> Path:
    return (
        ROOT
        / "tmp"
        / "reports"
        / "objc3c-public-workflow"
        / f"{validate_action}.json"
    )


def ensure_validate_report(validate_action: str) -> dict[str, Any]:
    report_path = workflow_report_path(validate_action)
    if report_path.is_file():
        report = load_json(report_path)
        if report.get("status") == "PASS":
            return report
    completed = subprocess.run(
        public_workflow_command(validate_action),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, f"{validate_action} command failed during integration validation")
    return load_json(report_path)


def main() -> int:
    workflow_surface = load_workflow_surface()
    validate_action = str(workflow_surface["validate_action"])
    required_steps = list(workflow_surface["validate_child_actions"])
    required_child_reports = dict(workflow_surface["required_child_reports"])
    validate_report_path = workflow_report_path(validate_action)
    workflow_report = ensure_validate_report(validate_action)
    expect(workflow_report.get("status") == "PASS", "validate-external-validation workflow report did not pass")
    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-external-validation workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == required_steps, "validate-external-validation workflow report step inventory drifted")

    seen_report_paths: set[str] = set()
    for step in steps:
        if not isinstance(step, dict):
            continue
        report_paths = step.get("report_paths", [])
        if isinstance(report_paths, list):
            for raw_path in report_paths:
                if isinstance(raw_path, str):
                    seen_report_paths.add(raw_path.replace("\\", "/"))

    child_reports: dict[str, dict[str, Any]] = {}
    for relative_path, contract_id in required_child_reports.items():
        expect(relative_path in seen_report_paths, f"validate-external-validation did not publish {relative_path}")
        payload = load_json(ROOT / relative_path)
        expect(payload.get("contract_id") == contract_id, f"external-validation child report contract drifted for {relative_path}")
        expect(payload.get("status") == "PASS", f"external-validation child report did not pass: {relative_path}")
        child_reports[relative_path] = payload

    replay_summary = child_reports["tmp/reports/external-validation/intake-replay-summary.json"]
    replay_steps = replay_summary.get("replay_steps", [])
    expect(isinstance(replay_steps, list) and len(replay_steps) >= 2, "external-validation replay summary drifted")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "workflow_surface": repo_rel(WORKFLOW_SURFACE),
        "workflow_report_path": repo_rel(validate_report_path),
        "required_steps": required_steps,
        "child_report_paths": list(required_child_reports.keys()),
        "child_reports": {
            relative_path: {
                "contract_id": report["contract_id"],
                "status": report["status"],
            }
            for relative_path, report in child_reports.items()
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-external-validation-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
