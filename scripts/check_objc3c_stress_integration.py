#!/usr/bin/env python3
"""Validate the integrated stress report set and workflow report."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture
from scripts.objc3c_workflow.public_command_api import public_workflow_command


ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "workflow_surface.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.integration.summary.v1"



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def workflow_report_path(validate_action: str) -> Path:
    return ROOT / "tmp" / "reports" / "objc3c-public-workflow" / f"{validate_action}.json"


def load_workflow_surface() -> dict[str, Any]:
    surface = load_json(WORKFLOW_SURFACE)
    expect(surface.get("contract_id") == "objc3c.stress.workflow.surface.v1", "stress workflow surface contract_id drifted")
    expect(surface.get("schema_version") == 1, "stress workflow surface schema_version drifted")
    return surface


def ensure_performance_dashboard_report(report_path: Path, build_action: str) -> dict[str, Any]:
    report = load_json(report_path) if report_path.is_file() else None
    if isinstance(report, dict) and isinstance(report.get("regression_gate"), dict):
        return report
    completed = run_capture(public_workflow_command(build_action))
    expect(completed.returncode == 0, f"{build_action} command failed during stress integration validation")
    return load_json(report_path)


def validate_performance_regression_gate(report: dict[str, Any], expected_gate_id: str) -> dict[str, Any]:
    gate = report.get("regression_gate")
    expect(isinstance(gate, dict), "performance dashboard missing regression_gate")
    expect(gate.get("gate_id") == expected_gate_id, "performance regression gate id drifted")
    release_status = gate.get("release_status")
    expect(release_status in {"release-ready", "caution", "blocked"}, "performance regression gate release_status drifted")
    blocking_breach_count = gate.get("blocking_breach_count")
    warning_breach_count = gate.get("warning_breach_count")
    expect(isinstance(blocking_breach_count, int) and blocking_breach_count >= 0, "performance regression gate blocking count drifted")
    expect(isinstance(warning_breach_count, int) and warning_breach_count >= 0, "performance regression gate warning count drifted")
    return {
        "gate_id": gate["gate_id"],
        "passed": bool(gate.get("passed")),
        "release_status": release_status,
        "blocking_breach_count": blocking_breach_count,
        "warning_breach_count": warning_breach_count,
    }


def ensure_validate_stress_report(validate_action: str) -> dict[str, Any]:
    report_path = workflow_report_path(validate_action)
    report = load_json(report_path) if report_path.is_file() else None
    if isinstance(report, dict) and report.get("status") == "PASS":
        return report
    completed = run_capture(public_workflow_command(validate_action))
    expect(completed.returncode == 0, f"{validate_action} command failed during stress integration validation")
    return load_json(report_path)


def main() -> int:
    workflow_surface = load_workflow_surface()
    validate_action = str(workflow_surface["validate_action"])
    required_steps = list(workflow_surface["validate_child_actions"])
    required_child_reports = dict(workflow_surface["required_child_reports"])
    performance_dashboard_report_path = ROOT / str(workflow_surface["performance_dashboard_report"])
    performance_dashboard_action = str(workflow_surface["performance_dashboard_action"])
    performance_regression_gate_id = str(workflow_surface["performance_regression_gate_id"])
    validate_report_path = workflow_report_path(validate_action)
    workflow_report = ensure_validate_stress_report(validate_action)
    expect(workflow_report.get("status") == "PASS", "validate-stress workflow report did not pass")
    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-stress workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == required_steps, "validate-stress workflow report step inventory drifted")

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
        expect(relative_path in seen_report_paths, f"validate-stress workflow report did not publish {relative_path}")
        payload = load_json(ROOT / relative_path)
        expect(payload.get("contract_id") == contract_id, f"stress child report contract drifted for {relative_path}")
        expect(payload.get("status") == "PASS", f"stress child report did not pass: {relative_path}")
        child_reports[relative_path] = payload

    performance_dashboard = ensure_performance_dashboard_report(
        performance_dashboard_report_path,
        performance_dashboard_action,
    )
    performance_gate = validate_performance_regression_gate(
        performance_dashboard,
        performance_regression_gate_id,
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "workflow_surface": repo_rel(WORKFLOW_SURFACE),
        "workflow_report_path": repo_rel(validate_report_path),
        "performance_dashboard_report_path": repo_rel(performance_dashboard_report_path),
        "performance_regression_gate": performance_gate,
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
    print("objc3c-stress-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
