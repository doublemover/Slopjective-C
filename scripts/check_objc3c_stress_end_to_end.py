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
from objc3c_tooling.public_runner import public_workflow_action_payload, public_workflow_has_actions
from objc3c_tooling.subprocesses import python_script_command


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
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
        if report.get("status") == "PASS":
            return report
    run_checked(
        python_script_command(ROOT / "scripts" / "check_objc3c_stress_integration.py"),
        "stress integration validator failed during end-to-end validation",
    )
    return load_json(INTEGRATION_REPORT)


def main() -> int:
    integration_report = ensure_integration_report()
    expect(integration_report.get("status") == "PASS", "stress integration report did not pass")

    nightly_payload = public_workflow_action_payload("test-nightly")
    expect(nightly_payload.get("action") == "test-nightly", "test-nightly describe payload drifted")
    expect(nightly_payload.get("validation_tier") == "nightly", "test-nightly validation_tier drifted")

    package_payload = load_json(PACKAGE_JSON)
    scripts = package_payload.get("scripts", {})
    expect(isinstance(scripts, dict), "package.json scripts drifted from object form")
    expect("objc3c" in scripts, "package.json missing objc3c package bridge")

    expect(
        public_workflow_has_actions(["test-nightly", "validate-stress"]),
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

    command_surface_text = COMMAND_SURFACE.read_text(encoding="utf-8")
    expect("validate-stress" in command_surface_text, "public command surface is missing validate-stress")
    expect(
        "validate-stress-integration" in command_surface_text,
        "public command surface is missing validate-stress-integration",
    )
    expect("validate-stress-end-to-end" in command_surface_text, "public command surface is missing validate-stress-end-to-end")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_report_path": repo_rel(INTEGRATION_REPORT),
        "nightly_description_action": nightly_payload["action"],
        "nightly_validation_tier": nightly_payload["validation_tier"],
        "nightly_includes_validate_stress": True,
        "command_surface_path": repo_rel(COMMAND_SURFACE),
        "package_bridge": "objc3c",
        "required_actions": [
            "check-stress-surface",
            "validate-stress",
            "validate-stress-integration",
            "validate-stress-end-to-end",
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-stress-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
