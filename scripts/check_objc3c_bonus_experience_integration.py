#!/usr/bin/env python3
"""Validate the live bonus-experience workflow across the integrated repo surface."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture
from objc3c_tooling.public_workflow_output import extract_line_value


ROOT = Path(__file__).resolve().parents[1]
BONUS_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "bonus-tool-integration.json"
SHOWCASE_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
GETTING_STARTED_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-integration-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "bonus-experiences" / "integration-summary.json"








def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        (
            "inspect-bonus-tool-integration",
            run_capture(public_workflow_command("inspect-bonus-tool-integration")),
        ),
        (
            "materialize-project-template",
            run_capture(
                public_workflow_command(
                    "materialize-project-template",
                    "--example",
                    "auroraBoard",
                )
            ),
        ),
        (
            "validate-showcase",
            run_capture(public_workflow_command("validate-showcase")),
        ),
        (
            "validate-getting-started",
            run_capture(public_workflow_command("validate-getting-started")),
        ),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    template_stdout = steps[1][1].stdout
    template_path_text = extract_line_value(template_stdout, "template_path:")
    harness_path_text = extract_line_value(template_stdout, "harness_path:")
    template_path = ROOT / template_path_text if template_path_text else Path()
    harness_path = ROOT / harness_path_text if harness_path_text else Path()

    expect(BONUS_INTEGRATION_REPORT.is_file(), f"missing bonus-tool integration report: {repo_rel(BONUS_INTEGRATION_REPORT)}", failures)
    expect(template_path_text != "", "materialize-project-template did not publish template_path", failures)
    expect(harness_path_text != "", "materialize-project-template did not publish harness_path", failures)
    expect(template_path.is_file(), f"missing template manifest: {template_path_text}", failures)
    expect(harness_path.is_file(), f"missing demo harness report: {harness_path_text}", failures)
    expect(SHOWCASE_INTEGRATION_REPORT.is_file(), f"missing showcase integration report: {repo_rel(SHOWCASE_INTEGRATION_REPORT)}", failures)
    expect(GETTING_STARTED_INTEGRATION_REPORT.is_file(), f"missing getting-started integration report: {repo_rel(GETTING_STARTED_INTEGRATION_REPORT)}", failures)

    bonus_integration = load_json(BONUS_INTEGRATION_REPORT) if BONUS_INTEGRATION_REPORT.is_file() else {}
    template_manifest = load_json(template_path) if template_path.is_file() else {}
    demo_harness = load_json(harness_path) if harness_path.is_file() else {}
    showcase_integration = load_json(SHOWCASE_INTEGRATION_REPORT) if SHOWCASE_INTEGRATION_REPORT.is_file() else {}
    getting_started_integration = load_json(GETTING_STARTED_INTEGRATION_REPORT) if GETTING_STARTED_INTEGRATION_REPORT.is_file() else {}

    expect(bonus_integration.get("contract_id") == "objc3c.bonus.tool.integration.surface.v1", "unexpected bonus-tool integration contract id", failures)
    expect(template_manifest.get("contract_id") == "objc3c.project.template.surface.v1", "unexpected project template contract id", failures)
    expect(demo_harness.get("contract_id") == "objc3c.project.template.demo.harness.v1", "unexpected demo harness contract id", failures)
    expect(demo_harness.get("ok") is True, "demo harness did not report ok=true", failures)
    expect(showcase_integration.get("contract_id") == "objc3c.showcase.integration.summary.v1", "unexpected showcase integration contract id", failures)
    expect(getting_started_integration.get("contract_id") == "objc3c.tutorial.getting-started.integration.summary.v1", "unexpected getting-started integration contract id", failures)

    payload = {
        "contract_id": "objc3c.bonus.experience.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_bonus_experience_integration.py",
        "child_report_paths": [
            repo_rel(BONUS_INTEGRATION_REPORT),
            template_path_text,
            harness_path_text,
            repo_rel(SHOWCASE_INTEGRATION_REPORT),
            repo_rel(GETTING_STARTED_INTEGRATION_REPORT),
        ],
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("bonus-experience-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("bonus-experience-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
