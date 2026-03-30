#!/usr/bin/env python3
"""Validate the integrated external-validation report set and workflow report."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
VALIDATE_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-external-validation.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "external-validation" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.integration.summary.v1"
REQUIRED_STEPS = [
    "check-external-validation-surface",
    "test-external-validation-replay",
    "publish-external-repro-corpus",
]
REQUIRED_CHILD_REPORTS = {
    "tmp/reports/external-validation/source-surface-summary.json": "objc3c.external_validation.source.surface.summary.v1",
    "tmp/reports/external-validation/intake-replay-summary.json": "objc3c.external_validation.intake.replay.summary.v1",
    "tmp/reports/external-validation/publication-summary.json": "objc3c.external_validation.publication.summary.v1",
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


def ensure_validate_report() -> dict[str, Any]:
    if VALIDATE_REPORT.is_file():
        report = load_json(VALIDATE_REPORT)
        if report.get("status") == "PASS":
            return report
    completed = subprocess.run(
        [sys.executable, str(RUNNER), "validate-external-validation"],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, "validate-external-validation command failed during integration validation")
    return load_json(VALIDATE_REPORT)


def main() -> int:
    workflow_report = ensure_validate_report()
    expect(workflow_report.get("status") == "PASS", "validate-external-validation workflow report did not pass")
    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-external-validation workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == REQUIRED_STEPS, "validate-external-validation workflow report step inventory drifted")

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
        "workflow_report_path": repo_rel(VALIDATE_REPORT),
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
    print("objc3c-external-validation-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
