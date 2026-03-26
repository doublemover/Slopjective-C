#!/usr/bin/env python3
"""Checker for M314-C001 task-runner and workflow API contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-C001" / "task_runner_workflow_api_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_task_runner_and_workflow_api_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c001_task_runner_and_workflow_api_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c001_task_runner_and_workflow_api_contract_and_architecture_freeze_contract.json"
PACKAGE_JSON = ROOT / "package.json"
README = ROOT / "README.md"
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    return parser.parse_args(argv)


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    package_text = read_text(PACKAGE_JSON)
    surface = package.get("objc3cCommandSurface", {})
    workflow_api = surface.get("workflowApi", {})
    readme = read_text(README)
    runner_text = read_text(RUNNER)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-task-runner-workflow-api/m314-c001-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-C001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("single public" in expectations.lower() and "workflow runner" in expectations.lower(), str(EXPECTATIONS_DOC), "M314-C001-EXP-02", "expectations missing single-runner note", failures)
    checks_passed += require("Freeze the API contract" in packet, str(PACKET_DOC), "M314-C001-PKT-01", "packet missing summary intent", failures)
    checks_passed += require("Next issue: `M314-C002`." in packet, str(PACKET_DOC), "M314-C001-PKT-02", "packet missing next issue", failures)

    checks_total += 10
    checks_passed += require(contract.get("mode") == "m314-c001-task-runner-workflow-api-contract-v1", str(CONTRACT_JSON), "M314-C001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-task-runner-workflow-api/m314-c001-v1", str(CONTRACT_JSON), "M314-C001-CON-02", "contract id drifted", failures)
    checks_passed += require(contract.get("depends_on") == "M314-B005", str(CONTRACT_JSON), "M314-C001-CON-03", "dependency drifted", failures)
    checks_passed += require(contract.get("runner_path") == "scripts/objc3c_public_workflow_runner.py", str(CONTRACT_JSON), "M314-C001-CON-04", "runner path drifted", failures)
    checks_passed += require(contract.get("public_scripts_budget_maximum") == 25, str(CONTRACT_JSON), "M314-C001-CON-05", "budget drifted", failures)
    checks_passed += require(contract.get("pass_through_actions") == ["compile-objc3c"], str(CONTRACT_JSON), "M314-C001-CON-06", "pass-through action set drifted", failures)
    checks_passed += require(len(contract.get("public_workflow_api", [])) == len(surface.get("publicScripts", [])), str(CONTRACT_JSON), "M314-C001-CON-07", "public workflow API count drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M314-C002", str(CONTRACT_JSON), "M314-C001-CON-08", "next issue drifted", failures)
    checks_passed += require(any("retired prototype compiler surface" in item.lower() for item in contract.get("invariants", [])), str(CONTRACT_JSON), "M314-C001-CON-09", "retired surface invariant missing", failures)
    checks_passed += require(any(entry.get("pass_through_args") is True for entry in contract.get("public_workflow_api", []) if entry.get("action") == "compile-objc3c"), str(CONTRACT_JSON), "M314-C001-CON-10", "compile pass-through contract missing", failures)

    checks_total += 9
    checks_passed += require(workflow_api.get("contractId") == contract.get("contract_id"), str(PACKAGE_JSON), "M314-C001-PKG-01", "package workflow contract id drifted", failures)
    checks_passed += require(workflow_api.get("runnerPath") == contract.get("runner_path"), str(PACKAGE_JSON), "M314-C001-PKG-02", "package runner path drifted", failures)
    checks_passed += require(workflow_api.get("publicActionCount") == len(contract.get("public_workflow_api", [])), str(PACKAGE_JSON), "M314-C001-PKG-03", "package public action count drifted", failures)
    checks_passed += require(workflow_api.get("passThroughActions") == ["compile:objc3c"], str(PACKAGE_JSON), "M314-C001-PKG-04", "package pass-through package script set drifted", failures)
    checks_passed += require("thin wrappers over" in readme and "scripts/objc3c_public_workflow_runner.py" in readme, str(README), "M314-C001-RD-01", "README missing workflow runner note", failures)
    checks_passed += require('if action == "compile-objc3c":' in runner_text and "return pwsh_file(COMPILE_PS1, *rest)" in runner_text, str(RUNNER), "M314-C001-RUN-01", "runner compile pass-through drifted", failures)
    checks_passed += require('if action == "build-default":' in runner_text and 'if action == "test-full":' in runner_text, str(RUNNER), "M314-C001-RUN-02", "runner action anchors drifted", failures)
    checks_passed += require("compiler/objc3c/semantic.py" not in runner_text, str(RUNNER), "M314-C001-RUN-03", "runner must not reference retired prototype path", failures)
    checks_passed += require('"check:objc3c:m314-c001-task-runner-and-workflow-api-contract-and-architecture-freeze"' in package_text and '"check:objc3c:m314-c001-lane-c-readiness"' in package_text, str(PACKAGE_JSON), "M314-C001-PKG-05", "package missing issue-local scripts", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "runner_path": contract["runner_path"],
        "public_action_count": len(contract["public_workflow_api"]),
        "pass_through_actions": contract["pass_through_actions"],
        "next_issue": "M314-C002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-C001 task-runner workflow API checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
