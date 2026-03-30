#!/usr/bin/env python3
"""Validate the integrated stress report set and workflow report."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
VALIDATE_STRESS_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-stress.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.integration.summary.v1"
REQUIRED_STEPS = [
    "check-stress-surface",
    "test-fuzz-safety",
    "test-lowering-runtime-stress",
    "test-mixed-module-differential",
    "test-stress-minimization",
    "test-stress-crash-triage",
]
REQUIRED_CHILD_REPORTS = {
    "tmp/reports/stress/source-surface-summary.json": "objc3c.stress.source.surface.summary.v1",
    "tmp/reports/stress/lowering-runtime-stress-summary.json": "objc3c.stress.lowering-runtime.summary.v1",
    "tmp/reports/stress/mixed-module-differential-summary.json": "objc3c.stress.mixed-module-differential.summary.v1",
    "tmp/reports/stress/minimization-summary.json": "objc3c.stress.minimization.summary.v1",
    "tmp/reports/stress/crash-triage-summary.json": "objc3c.stress.crash.triage.summary.v1",
}


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def ensure_validate_stress_report() -> dict[str, Any]:
    report = load_json(VALIDATE_STRESS_REPORT) if VALIDATE_STRESS_REPORT.is_file() else None
    if isinstance(report, dict) and report.get("status") == "PASS":
        return report
    completed = subprocess.run(
        [sys.executable, str(RUNNER), "validate-stress"],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, "validate-stress command failed during stress integration validation")
    return load_json(VALIDATE_STRESS_REPORT)


def main() -> int:
    workflow_report = ensure_validate_stress_report()
    expect(workflow_report.get("status") == "PASS", "validate-stress workflow report did not pass")
    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-stress workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == REQUIRED_STEPS, "validate-stress workflow report step inventory drifted")

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
    for relative_path, contract_id in REQUIRED_CHILD_REPORTS.items():
        expect(relative_path in seen_report_paths, f"validate-stress workflow report did not publish {relative_path}")
        payload = load_json(ROOT / relative_path)
        expect(payload.get("contract_id") == contract_id, f"stress child report contract drifted for {relative_path}")
        expect(payload.get("status") == "PASS", f"stress child report did not pass: {relative_path}")
        child_reports[relative_path] = payload

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "workflow_report_path": repo_rel(VALIDATE_STRESS_REPORT),
        "required_steps": REQUIRED_STEPS,
        "child_report_paths": list(REQUIRED_CHILD_REPORTS.keys()),
        "child_reports": {
            relative_path: {
                "contract_id": report["contract_id"],
                "status": report["status"],
            }
            for relative_path, report in child_reports.items()
        },
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-stress-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
