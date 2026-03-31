#!/usr/bin/env python3
"""Validate security-hardening entrypoints, command-surface sync, and publication artifacts end to end."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-security-hardening.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "workflow_surface.json"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "schema-surface-summary.json"
RESPONSE_DRILL_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-drill-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "integration-summary.json"
POSTURE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "publication-summary.json"
COMMAND_SURFACE = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "end-to-end-summary.json"

REQUIRED_STEPS = [
    "check-security-response-drill",
    "check-security-hardening-surface",
    "check-security-hardening-schema-surface",
    "build-security-posture",
    "publish-security-advisories",
]
REQUIRED_SCRIPTS = [
    "check:objc3c:security-hardening:surface",
    "check:objc3c:security-hardening:schemas",
    "inspect:objc3c:security-posture",
    "publish:objc3c:security-advisories",
    "test:objc3c:security-hardening",
    "test:objc3c:security-hardening:e2e",
]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {path} did not contain an object")
    return payload


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def ensure_workflow_report() -> dict[str, Any]:
    completed = subprocess.run(
        [sys.executable, str(RUNNER), "validate-security-hardening"],
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

    for path in (SOURCE_SUMMARY, SCHEMA_SUMMARY, RESPONSE_DRILL_SUMMARY, POSTURE_SUMMARY, PUBLICATION_SUMMARY):
        expect(path.is_file(), f"missing required security artifact {repo_rel(path)}")
        payload = load_json(path)
        expect(payload.get("status") == "PASS", f"security artifact did not pass: {repo_rel(path)}")

    integration = subprocess.run(
        [sys.executable, str(ROOT / "scripts" / "check_objc3c_security_hardening_integration.py")],
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

    command_surface_text = COMMAND_SURFACE.read_text(encoding="utf-8")
    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    expect(isinstance(package_scripts, dict), "package.json scripts drifted from object")
    for script_name in REQUIRED_SCRIPTS:
        expect(script_name in command_surface_text, f"public command surface missing {script_name}")
        expect(script_name in package_scripts, f"package.json missing {script_name}")

    payload = {
        "contract_id": "objc3c.security.hardening.end_to_end.summary.v1",
        "status": "PASS",
        "workflow_report_path": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "child_report_paths": [
            repo_rel(SOURCE_SUMMARY),
            repo_rel(SCHEMA_SUMMARY),
            repo_rel(RESPONSE_DRILL_SUMMARY),
            repo_rel(INTEGRATION_SUMMARY),
            repo_rel(POSTURE_SUMMARY),
            repo_rel(PUBLICATION_SUMMARY),
        ],
        "security_state": publication.get("security_state"),
        "publication_summary_path": repo_rel(PUBLICATION_SUMMARY),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-security-hardening-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
