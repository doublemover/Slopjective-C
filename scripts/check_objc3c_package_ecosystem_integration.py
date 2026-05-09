#!/usr/bin/env python3
"""Validate the live package ecosystem workflow against user-shaped workspaces."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_timed
from package_ecosystem_contracts import package_ecosystem_owner_payload


ROOT = Path(__file__).resolve().parents[1]
AUTHORING_SUMMARY = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-authoring-workflow-summary.json"
LOCK_SUMMARY = ROOT / "tmp" / "reports" / "package-ecosystem" / "package-lock-summary.json"
APP_ARCH_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "runnable-template-canonical-app-summary.json"
STDLIB_PROGRAM_SUMMARY = ROOT / "tmp" / "reports" / "stdlib" / "program-integration-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "integration-summary.json"

STEPS = [
    ("package-authoring-workflow", python_script_command("scripts/check_objc3c_package_authoring_workflow.py")),
    ("application-architecture-integration", python_script_command("scripts/check_objc3c_application_architecture_integration.py")),
    ("stdlib-program-integration", python_script_command("scripts/check_objc3c_stdlib_program_integration.py")),
]



def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "duration_ms": result.duration_ms,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }



def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    failures: list[str] = []
    steps = []
    for name, command in STEPS:
        step = run_step(name, command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"{name} failed", failures)
        if step["exit_code"] != 0:
            break

    for path in (AUTHORING_SUMMARY, LOCK_SUMMARY, APP_ARCH_SUMMARY, STDLIB_PROGRAM_SUMMARY):
        expect(path.is_file(), f"missing expected package ecosystem integration child report {repo_rel(path)}", failures)

    authoring_summary = load_json(AUTHORING_SUMMARY) if AUTHORING_SUMMARY.is_file() else {}
    lock_summary = load_json(LOCK_SUMMARY) if LOCK_SUMMARY.is_file() else {}
    app_arch_summary = load_json(APP_ARCH_SUMMARY) if APP_ARCH_SUMMARY.is_file() else {}
    stdlib_program_summary = load_json(STDLIB_PROGRAM_SUMMARY) if STDLIB_PROGRAM_SUMMARY.is_file() else {}

    expect(summary_passes(authoring_summary), "package authoring workflow summary did not report PASS", failures)
    expect(summary_passes(lock_summary), "package lock summary did not report PASS", failures)
    expect(summary_passes(app_arch_summary), "application architecture integration summary did not report PASS", failures)
    expect(summary_passes(stdlib_program_summary), "stdlib program integration summary did not report PASS", failures)
    expect(lock_summary.get("package_count", 0) >= 8, "package lock did not include stdlib and showcase packages", failures)
    expect(lock_summary.get("dependency_count", 0) >= 7, "package lock did not include showcase dependency edges", failures)
    expect(app_arch_summary.get("workflow_actions") == [
        "materialize-project-template",
        "materialize-canonical-application-workspace",
        "validate-showcase",
        "validate-stdlib-program",
        "validate-application-architecture",
    ], "application architecture workflow action chain drifted", failures)

    payload = {
        "contract_id": "objc3c.package_ecosystem.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_package_ecosystem_integration.py",
        "owner_policy": package_ecosystem_owner_payload(),
        "blocker_metadata": {
            "blocker_owner": "package-ecosystem-blockers",
            "blocking_conditions": [
                "child package ecosystem workflow failed",
                "package lock summary failed source-owned replay checks",
                "application architecture package surface drifted",
                "stdlib package program surface drifted",
            ],
        },
        "workflow_actions": [
            "build-package-lock",
            "validate-package-authoring",
            "validate-application-architecture",
            "validate-stdlib-program",
            "validate-package-ecosystem",
        ],
        "child_report_paths": [
            repo_rel(AUTHORING_SUMMARY),
            repo_rel(LOCK_SUMMARY),
            repo_rel(APP_ARCH_SUMMARY),
            repo_rel(STDLIB_PROGRAM_SUMMARY),
        ],
        "steps": steps,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("package-ecosystem-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("package-ecosystem-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
