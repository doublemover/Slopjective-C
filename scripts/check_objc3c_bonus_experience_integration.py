#!/usr/bin/env python3
"""Validate the live bonus-experience workflow across the integrated repo surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
BONUS_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "bonus-tool-integration.json"
SHOWCASE_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
GETTING_STARTED_INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-integration-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "bonus-experiences" / "integration-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def extract_line_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        (
            "inspect-bonus-tool-integration",
            run_capture([sys.executable, str(PUBLIC_RUNNER), "inspect-bonus-tool-integration"]),
        ),
        (
            "materialize-project-template",
            run_capture(
                [
                    sys.executable,
                    str(PUBLIC_RUNNER),
                    "materialize-project-template",
                    "--example",
                    "auroraBoard",
                ]
            ),
        ),
        (
            "validate-showcase",
            run_capture([sys.executable, str(PUBLIC_RUNNER), "validate-showcase"]),
        ),
        (
            "validate-getting-started",
            run_capture([sys.executable, str(PUBLIC_RUNNER), "validate-getting-started"]),
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
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
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
