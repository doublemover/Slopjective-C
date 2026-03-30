#!/usr/bin/env python3
"""Validate the integrated public-conformance reporting workflow report."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
VALIDATE_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-public-conformance-reporting.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "public-conformance" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.integration.summary.v1"
REQUIRED_STEPS = [
    "check-public-conformance-reporting-surface",
    "check-public-conformance-schema-surface",
    "build-public-conformance-scorecard",
    "publish-public-conformance-report",
]
REQUIRED_CHILD_REPORTS = {
    "tmp/reports/public-conformance/source-surface-summary.json": "objc3c.public_conformance_reporting.source.surface.summary.v1",
    "tmp/reports/public-conformance/schema-surface-summary.json": "objc3c.public_conformance_reporting.schema.surface.summary.v1",
    "tmp/reports/public-conformance/scorecard-summary.json": "objc3c.public_conformance_reporting.scorecard.summary.v1",
    "tmp/reports/public-conformance/public-summary.json": "objc3c.public_conformance_reporting.summary.v1",
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
        [sys.executable, str(RUNNER), "validate-public-conformance-reporting"],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, "validate-public-conformance-reporting command failed during integration validation")
    return load_json(VALIDATE_REPORT)


def main() -> int:
    workflow_report = ensure_validate_report()
    expect(workflow_report.get("status") == "PASS", "validate-public-conformance-reporting workflow report did not pass")
    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-public-conformance-reporting workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == REQUIRED_STEPS, "validate-public-conformance-reporting workflow report step inventory drifted")

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
        expect(relative_path in seen_report_paths, f"validate-public-conformance-reporting did not publish {relative_path}")
        payload = load_json(ROOT / relative_path)
        expect(payload.get("contract_id") == contract_id, f"public-conformance child report contract drifted for {relative_path}")
        expect(payload.get("status") == "PASS", f"public-conformance child report did not pass: {relative_path}")
        child_reports[relative_path] = payload

    scorecard = child_reports["tmp/reports/public-conformance/scorecard-summary.json"]
    public_summary = child_reports["tmp/reports/public-conformance/public-summary.json"]
    expect(public_summary.get("score") == scorecard.get("score"), "public summary score drifted from scorecard")
    expect(public_summary.get("badge") == scorecard.get("badge"), "public summary badge drifted from scorecard")
    expect(public_summary.get("public_status") == scorecard.get("public_status"), "public summary public_status drifted from scorecard")

    report_markdown_path = public_summary.get("report_markdown_path")
    expect(isinstance(report_markdown_path, str) and report_markdown_path, "public summary is missing report_markdown_path")
    expect((ROOT / report_markdown_path).is_file(), "public-conformance markdown report is missing")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "workflow_report_path": repo_rel(VALIDATE_REPORT),
        "required_steps": REQUIRED_STEPS,
        "child_report_paths": list(REQUIRED_CHILD_REPORTS.keys()),
        "markdown_report_path": report_markdown_path,
        "score": scorecard.get("score"),
        "badge": scorecard.get("badge"),
        "public_status": scorecard.get("public_status"),
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-public-conformance-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
