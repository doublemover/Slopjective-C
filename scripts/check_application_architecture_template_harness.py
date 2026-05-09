#!/usr/bin/env python3
"""Validate the application-architecture template harness on the live public path."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.public_runner import public_workflow_command
from objc3c_tooling.subprocesses import run_timed


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "project_template_workspace_semantics.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
DEFAULT_EXAMPLE = "auroraBoard"




def run_step(name: str, command: list[str]) -> dict[str, object]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
        "stdout": completed.stdout,
    }


def extract_output_line(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    step = run_step(
        "materialize-project-template",
        public_workflow_command(
            "materialize-project-template",
            "--example",
            DEFAULT_EXAMPLE,
        ),
    )

    failures: list[str] = []
    expect(step["exit_code"] == 0, "materialize-project-template failed", failures)
    template_path_text = extract_output_line(str(step["stdout"]), "template_path:")
    harness_path_text = extract_output_line(str(step["stdout"]), "harness_path:")
    expect(bool(template_path_text), "materialize-project-template did not publish template_path", failures)
    expect(bool(harness_path_text), "materialize-project-template did not publish harness_path", failures)

    template_payload: dict[str, Any] = {}
    harness_payload: dict[str, Any] = {}
    template_path = ROOT / template_path_text if template_path_text else ROOT
    harness_path = ROOT / harness_path_text if harness_path_text else ROOT
    if template_path_text:
        expect(template_path.is_file(), f"missing template manifest {template_path_text}", failures)
        if template_path.is_file():
            template_payload = load_json(template_path)
    if harness_path_text:
        expect(harness_path.is_file(), f"missing harness manifest {harness_path_text}", failures)
        if harness_path.is_file():
            harness_payload = load_json(harness_path)

    if template_payload:
        expect(
            template_payload.get("contract_id") == contract["template_contract_id"],
            "template contract id drifted",
            failures,
        )
        contract_refs = template_payload.get("application_architecture_testing_contracts", {})
        expect(
            isinstance(contract_refs, dict)
            and contract_refs.get("project_template_workspace")
            == repo_rel(CONTRACT_PATH),
            "template manifest did not publish the project template workspace contract",
            failures,
        )
        expect(
            template_payload.get("source_origin") == "showcase/auroraBoard/main.objc3",
            "template manifest source origin drifted from the canonical example",
            failures,
        )
        expect(
            template_payload.get("public_actions") == contract["required_actions"],
            "template manifest public actions drifted from the canonical template semantics",
            failures,
        )
        expect(
            template_payload.get("tutorial_guides")
            == [
                "docs/tutorials/getting_started.md",
                "docs/tutorials/build_run_verify.md",
                "docs/tutorials/guided_walkthrough.md",
            ],
            "template manifest tutorial guides drifted",
            failures,
        )

    if harness_payload:
        expect(
            harness_payload.get("contract_id") == contract["template_harness_contract_id"],
            "template harness contract id drifted",
            failures,
        )
        expect(harness_payload.get("ok") is True, "template harness reported ok=false", failures)
        for field in ("integration_report", "playground_workspace", "benchmark_report"):
            value = harness_payload.get(field)
            expect(isinstance(value, str) and bool(value), f"template harness missing {field}", failures)
            if isinstance(value, str) and value:
                expect((ROOT / value).exists(), f"template harness published missing path {value}", failures)

    payload = {
        "contract_id": "objc3c.application.architecture.testing.template_harness.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "template_workspace_contract": repo_rel(CONTRACT_PATH),
        "template_path": template_path_text,
        "harness_path": harness_path_text,
        "template_contract_id": template_payload.get("contract_id"),
        "harness_contract_id": harness_payload.get("contract_id"),
        "public_actions": template_payload.get("public_actions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-template-harness: PASS" if not failures else "application-architecture-template-harness: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
