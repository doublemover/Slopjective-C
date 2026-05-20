#!/usr/bin/env python3
"""Validate the public end-to-end stress workflow surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_action_payload, public_workflow_has_actions
from objc3c_tooling.subprocesses import python_script_command


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "workflow_surface.json"
COMMAND_SURFACE = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
PACKAGE_JSON = ROOT / "package.json"
RENDER_COMMAND_SURFACE = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
TASK_HYGIENE_GATE = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "stress" / "end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.end-to-end.summary.v1"



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def run_checked(command: list[str], message: str) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, message)
    return completed


def ensure_integration_report() -> dict[str, Any]:
    if INTEGRATION_REPORT.is_file():
        report = load_json(INTEGRATION_REPORT)
        if report.get("status") == "PASS" and isinstance(report.get("performance_regression_gate"), dict):
            return report
    run_checked(
        python_script_command(ROOT / "scripts" / "check_objc3c_stress_integration.py"),
        "stress integration validator failed during end-to-end validation",
    )
    return load_json(INTEGRATION_REPORT)


def load_workflow_surface() -> dict[str, Any]:
    surface = load_json(WORKFLOW_SURFACE)
    expect(surface.get("contract_id") == "objc3c.stress.workflow.surface.v1", "stress workflow surface contract_id drifted")
    expect(surface.get("schema_version") == 1, "stress workflow surface schema_version drifted")
    return surface


def main() -> int:
    workflow_surface = load_workflow_surface()
    integration_report = ensure_integration_report()
    expect(integration_report.get("status") == "PASS", "stress integration report did not pass")
    performance_gate = integration_report.get("performance_regression_gate")
    expect(isinstance(performance_gate, dict), "stress integration report missing performance regression gate")
    expect(
        performance_gate.get("gate_id") == workflow_surface.get("performance_regression_gate_id"),
        "stress integration performance regression gate id drifted",
    )

    nightly_action = str(workflow_surface["nightly_action"])
    validate_action = str(workflow_surface["validate_action"])
    required_actions = list(workflow_surface["required_actions"])
    command_surface_path = ROOT / str(workflow_surface["command_surface"])
    nightly_payload = public_workflow_action_payload(nightly_action)
    expect(nightly_payload.get("action") == nightly_action, "test-nightly describe payload drifted")
    expect(nightly_payload.get("validation_tier") == "nightly", "test-nightly validation_tier drifted")

    package_payload = load_json(PACKAGE_JSON)
    scripts = package_payload.get("scripts", {})
    expect(isinstance(scripts, dict), "package.json scripts drifted from object form")
    expect("objc3c" in scripts, "package.json missing objc3c package bridge")

    expect(
        public_workflow_has_actions([nightly_action, *required_actions]),
        "workflow registry no longer exposes test-nightly and validate-stress",
    )

    run_checked(
        python_script_command(RENDER_COMMAND_SURFACE, "--check"),
        "public command surface check failed during stress end-to-end validation",
    )
    run_checked(
        python_script_command(TASK_HYGIENE_GATE),
        "task hygiene gate failed during stress end-to-end validation",
    )

    command_surface_text = command_surface_path.read_text(encoding="utf-8")
    for action in required_actions:
        expect(action in command_surface_text, f"public command surface is missing {action}")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "workflow_surface": repo_rel(WORKFLOW_SURFACE),
        "integration_report_path": repo_rel(INTEGRATION_REPORT),
        "nightly_description_action": nightly_payload["action"],
        "nightly_validation_tier": nightly_payload["validation_tier"],
        "nightly_includes_validate_stress": validate_action in required_actions,
        "command_surface_path": repo_rel(command_surface_path),
        "package_bridge": "objc3c",
        "required_actions": required_actions,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-stress-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
