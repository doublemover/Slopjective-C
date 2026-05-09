#!/usr/bin/env python3
"""Validate template and canonical application integration on the live workflow."""

from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_any as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
TEMPLATE_HARNESS_PY = ROOT / "scripts" / "check_application_architecture_template_harness.py"
CANONICAL_WORKSPACE_PY = ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
TEMPLATE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
CANONICAL_SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
SHOWCASE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
STDLIB_PROGRAM_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-integration-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "runnable-template-canonical-app-summary.json"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def require_executed_summary(
    *,
    name: str,
    summary_path: Path,
    failures: list[str],
    command: list[str],
) -> dict[str, Any]:
    step_result: dict[str, Any] = {
        "name": name,
        "command": command,
        "summary_path": repo_rel(summary_path),
        "mode": "executed",
    }
    result = run_capture(command)
    step_result["exit_code"] = result.returncode
    expect(result.returncode == 0, f"{name} failed", failures)

    if not summary_path.is_file():
        failures.append(f"{name} missing expected summary {repo_rel(summary_path)}")
        step_result["summary_ok"] = False
        return step_result

    summary = load_json(summary_path)
    step_result["summary_ok"] = summary_passes(summary)
    step_result["summary_status"] = summary.get("status", summary.get("ok"))
    if not step_result["summary_ok"]:
        failures.append(f"{name} summary did not report PASS")
    return summary


def main() -> int:
    failures: list[str] = []
    step_results = []

    template_command = python_script_command(TEMPLATE_HARNESS_PY)
    template_summary = require_executed_summary(
        name="template-harness",
        command=template_command,
        summary_path=TEMPLATE_SUMMARY_PATH,
        failures=failures,
    )
    step_results.append(
        {
            "name": "template-harness",
            "command": template_command,
            "summary_path": repo_rel(TEMPLATE_SUMMARY_PATH),
            "mode": "executed",
            "summary_ok": summary_passes(template_summary) if isinstance(template_summary, dict) else False,
            "summary_status": template_summary.get("status", template_summary.get("ok")) if isinstance(template_summary, dict) else None,
        }
    )
    canonical_command = python_script_command(CANONICAL_WORKSPACE_PY)
    canonical_summary = require_executed_summary(
        name="canonical-workspace",
        command=canonical_command,
        summary_path=CANONICAL_SUMMARY_PATH,
        failures=failures,
    )
    step_results.append(
        {
            "name": "canonical-workspace",
            "command": canonical_command,
            "summary_path": repo_rel(CANONICAL_SUMMARY_PATH),
            "mode": "executed",
            "summary_ok": summary_passes(canonical_summary) if isinstance(canonical_summary, dict) else False,
            "summary_status": canonical_summary.get("status", canonical_summary.get("ok")) if isinstance(canonical_summary, dict) else None,
        }
    )

    showcase_command = python_script_command(SHOWCASE_INTEGRATION_PY)
    showcase_summary = require_executed_summary(
        name="showcase-integration",
        command=showcase_command,
        summary_path=SHOWCASE_SUMMARY_PATH,
        failures=failures,
    )
    step_results.append(
        {
            "name": "showcase-integration",
            "command": showcase_command,
            "summary_path": repo_rel(SHOWCASE_SUMMARY_PATH),
            "mode": "executed",
            "summary_ok": summary_passes(showcase_summary) if isinstance(showcase_summary, dict) else False,
            "summary_status": showcase_summary.get("status", showcase_summary.get("ok")) if isinstance(showcase_summary, dict) else None,
        }
    )
    if not summary_passes(showcase_summary):
        expect(summary_passes(showcase_summary), "showcase integration summary did not report PASS", failures)

    stdlib_program_command = python_script_command(STDLIB_PROGRAM_INTEGRATION_PY)
    stdlib_program_summary = require_executed_summary(
        name="stdlib-program-integration",
        command=stdlib_program_command,
        summary_path=STDLIB_PROGRAM_SUMMARY_PATH,
        failures=failures,
    )
    step_results.append(
        {
            "name": "stdlib-program-integration",
            "command": stdlib_program_command,
            "summary_path": repo_rel(STDLIB_PROGRAM_SUMMARY_PATH),
            "mode": "executed",
            "summary_ok": summary_passes(stdlib_program_summary) if isinstance(stdlib_program_summary, dict) else False,
            "summary_status": stdlib_program_summary.get("status", stdlib_program_summary.get("ok")) if isinstance(stdlib_program_summary, dict) else None,
        }
    )
    if not summary_passes(stdlib_program_summary):
        expect(summary_passes(stdlib_program_summary), "stdlib program integration summary did not report PASS", failures)

    expect(template_summary.get("status") == "PASS", "template harness summary did not report PASS", failures)
    expect(
        canonical_summary.get("status") == "PASS",
        "canonical application workspace summary did not report PASS",
        failures,
    )
    expect(
        canonical_summary.get("example_count") == 3,
        "canonical application workspace did not include all showcase examples",
        failures,
    )
    expect(
        canonical_summary.get("architecture_layer_count") == 4,
        "canonical application workspace layer count drifted from the canonical contract",
        failures,
    )
    expect(
        showcase_summary.get("contract_id") == "objc3c.showcase.integration.summary.v1",
        "showcase integration summary contract drifted",
        failures,
    )
    expect(
        stdlib_program_summary.get("contract_id") == "objc3c.stdlib.program.integration.summary.v1",
        "stdlib program integration summary contract drifted",
        failures,
    )

    payload = {
        "contract_id": "objc3c.application.architecture.testing.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_application_architecture_integration.py",
        "workflow_actions": [
            "materialize-project-template",
            "materialize-canonical-application-workspace",
            "validate-showcase",
            "validate-stdlib-program",
            "validate-application-architecture",
        ],
        "child_report_paths": [
            repo_rel(TEMPLATE_SUMMARY_PATH),
            repo_rel(CANONICAL_SUMMARY_PATH),
            repo_rel(SHOWCASE_SUMMARY_PATH),
            repo_rel(STDLIB_PROGRAM_SUMMARY_PATH),
        ],
        "steps": step_results,
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("application-architecture-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("application-architecture-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
