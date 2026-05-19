#!/usr/bin/env python3
"""Validate security-hardening entrypoints, command-surface sync, and publication artifacts end to end."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names, public_workflow_command
from objc3c_tooling.subprocesses import python_script_command


ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-security-hardening.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "workflow_surface.json"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "schema-surface-summary.json"
RESPONSE_DRILL_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-drill-summary.json"
RUNTIME_HARDENING_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "runtime-hardening-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "integration-summary.json"
POSTURE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "publication-summary.json"
COMMAND_SURFACE = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "end-to-end-summary.json"

REQUIRED_STEPS = [
    "check-security-response-drill",
    "check-security-runtime-hardening",
    "check-security-hardening-surface",
    "check-security-hardening-schema-surface",
    "build-security-posture",
    "publish-security-advisories",
]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def ensure_workflow_report() -> dict[str, Any]:
    completed = subprocess.run(
        public_workflow_command("validate-security-hardening"),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, "validate-security-hardening command failed during end-to-end validation")
    return load_json(WORKFLOW_REPORT)


def main() -> int:
    workflow_report = ensure_workflow_report()
    workflow_surface = load_json(WORKFLOW_SURFACE)
    expect(workflow_report.get("status") == "PASS", "validate-security-hardening workflow report did not pass")
    expect(workflow_surface.get("validate_action") == "validate-security-hardening", "workflow surface drifted")

    steps = workflow_report.get("steps", [])
    expect(isinstance(steps, list), "validate-security-hardening workflow report steps drifted")
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    expect(step_actions == REQUIRED_STEPS, "validate-security-hardening workflow step inventory drifted")

    for path in (SOURCE_SUMMARY, SCHEMA_SUMMARY, RESPONSE_DRILL_SUMMARY, RUNTIME_HARDENING_SUMMARY, POSTURE_SUMMARY, PUBLICATION_SUMMARY):
        expect(path.is_file(), f"missing required security artifact {repo_rel(path)}")
        payload = load_json(path)
        expect(payload.get("status") == "PASS", f"security artifact did not pass: {repo_rel(path)}")

    integration = subprocess.run(
        python_script_command(ROOT / "scripts" / "check_objc3c_security_hardening_integration.py"),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if integration.stdout:
        sys.stdout.write(integration.stdout)
    if integration.stderr:
        sys.stderr.write(integration.stderr)
    expect(integration.returncode == 0, "security-hardening integration validation failed")
    expect(INTEGRATION_SUMMARY.is_file(), "security-hardening integration summary is missing")

    publication = load_json(PUBLICATION_SUMMARY)
    posture_summary = load_json(POSTURE_SUMMARY)
    expect(publication.get("security_state") == posture_summary.get("security_state"), "publication drifted from posture state")

    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    expect(isinstance(package_scripts, dict), "package.json scripts drifted from object")
    package_bridge = str(workflow_surface["package_bridge"])
    expect(package_bridge in package_scripts, f"package.json missing package bridge {package_bridge}")
    registered_actions = set(public_workflow_action_names())
    for action in workflow_surface["required_actions"]:
        expect(str(action) in registered_actions, f"workflow registry missing action {action}")

    payload = {
        "contract_id": "objc3c.security.hardening.end_to_end.summary.v1",
        "status": "PASS",
        "workflow_report_path": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "child_report_paths": [
            repo_rel(SOURCE_SUMMARY),
            repo_rel(SCHEMA_SUMMARY),
            repo_rel(RESPONSE_DRILL_SUMMARY),
            repo_rel(RUNTIME_HARDENING_SUMMARY),
            repo_rel(INTEGRATION_SUMMARY),
            repo_rel(POSTURE_SUMMARY),
            repo_rel(PUBLICATION_SUMMARY),
        ],
        "security_state": publication.get("security_state"),
        "package_bridge": package_bridge,
        "required_actions": workflow_surface["required_actions"],
        "publication_summary_path": repo_rel(PUBLICATION_SUMMARY),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-security-hardening-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
