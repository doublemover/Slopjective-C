#!/usr/bin/env python3
"""Checker for M314-C002 parameterized task runner implementation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-C002" / "parameterized_task_runner_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_parameterized_task_runner_implementation_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c002_parameterized_task_runner_implementation_core_feature_implementation_packet.md"
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_c002_parameterized_task_runner_implementation_core_feature_implementation_registry.json"
PACKAGE_JSON = ROOT / "package.json"
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


def run_json(*args: str) -> dict[str, object]:
    result = subprocess.run(
        [sys.executable, str(RUNNER), *args],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    )
    return json.loads(result.stdout)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    registry = json.loads(read_text(REGISTRY_JSON))
    package = json.loads(read_text(PACKAGE_JSON))
    package_text = read_text(PACKAGE_JSON)
    workflow_api = package.get("objc3cCommandSurface", {}).get("workflowApi", {})
    runner_text = read_text(RUNNER)
    live_list = run_json("--list-json")
    live_compile = run_json("--describe", "compile-objc3c")
    extra_arg_probe = subprocess.run(
        [sys.executable, str(RUNNER), "build-default", "unexpected"],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
    )

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-parameterized-task-runner/m314-c002-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-C002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("--list-json" in expectations and "--describe <action>" in expectations, str(EXPECTATIONS_DOC), "M314-C002-EXP-02", "expectations missing introspection notes", failures)
    checks_passed += require("parameterized action" in packet and "registry" in packet, str(PACKET_DOC), "M314-C002-PKT-01", "packet missing registry implementation note", failures)
    checks_passed += require("Next issue: `M314-C003`." in packet, str(PACKET_DOC), "M314-C002-PKT-02", "packet missing next issue", failures)

    checks_total += 8
    checks_passed += require(registry.get("mode") == "m314-c002-parameterized-task-runner-v1", str(REGISTRY_JSON), "M314-C002-REG-01", "mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-cleanup-parameterized-task-runner/m314-c002-v1", str(REGISTRY_JSON), "M314-C002-REG-02", "contract id drifted", failures)
    checks_passed += require(registry.get("depends_on") == "M314-C001", str(REGISTRY_JSON), "M314-C002-REG-03", "dependency drifted", failures)
    checks_passed += require(registry.get("action_count") == 17, str(REGISTRY_JSON), "M314-C002-REG-04", "action count drifted", failures)
    checks_passed += require(registry.get("pass_through_actions") == ["compile-objc3c"], str(REGISTRY_JSON), "M314-C002-REG-05", "pass-through action set drifted", failures)
    checks_passed += require(registry.get("package_workflow_api_fields", {}).get("runnerMode") == "m314-c002-parameterized-task-runner-v1", str(REGISTRY_JSON), "M314-C002-REG-06", "runner mode drifted", failures)
    checks_passed += require(registry.get("package_workflow_api_fields", {}).get("introspectionCommand") == "python scripts/objc3c_public_workflow_runner.py --list-json", str(REGISTRY_JSON), "M314-C002-REG-07", "introspection command drifted", failures)
    checks_passed += require(registry.get("next_issue") == "M314-C003", str(REGISTRY_JSON), "M314-C002-REG-08", "next issue drifted", failures)

    checks_total += 10
    checks_passed += require("@dataclass(frozen=True)\nclass ActionSpec" in runner_text, str(RUNNER), "M314-C002-RUN-01", "runner missing ActionSpec dataclass", failures)
    checks_passed += require("ACTION_SPECS: dict[str, ActionSpec]" in runner_text, str(RUNNER), "M314-C002-RUN-02", "runner missing action registry", failures)
    checks_passed += require("ACTION_HANDLERS: dict[str, ActionHandler]" in runner_text, str(RUNNER), "M314-C002-RUN-03", "runner missing action handler registry", failures)
    checks_passed += require("def list_actions_payload()" in runner_text and "def describe_action_payload(action: str)" in runner_text, str(RUNNER), "M314-C002-RUN-04", "runner missing introspection helpers", failures)
    checks_passed += require(live_list.get("mode") == registry.get("mode"), str(RUNNER), "M314-C002-LIVE-01", "live list mode drifted", failures)
    checks_passed += require(live_list.get("action_count") == registry.get("action_count"), str(RUNNER), "M314-C002-LIVE-02", "live action count drifted", failures)
    checks_passed += require(len(live_list.get("actions", [])) == registry.get("action_count"), str(RUNNER), "M314-C002-LIVE-03", "live action listing count drifted", failures)
    checks_passed += require(live_compile.get("action") == "compile-objc3c" and live_compile.get("pass_through_args") is True, str(RUNNER), "M314-C002-LIVE-04", "compile describe output drifted", failures)
    checks_passed += require(extra_arg_probe.returncode == 2 and "does not accept extra arguments" in extra_arg_probe.stderr, str(RUNNER), "M314-C002-LIVE-05", "fixed-shape argument rejection missing", failures)
    checks_passed += require("compiler/objc3c/semantic.py" not in runner_text, str(RUNNER), "M314-C002-RUN-05", "runner must not reference retired prototype path", failures)

    checks_total += 5
    checks_passed += require(workflow_api.get("runnerMode") == registry["package_workflow_api_fields"]["runnerMode"], str(PACKAGE_JSON), "M314-C002-PKG-01", "package runner mode drifted", failures)
    checks_passed += require(workflow_api.get("introspectionCommand") == registry["package_workflow_api_fields"]["introspectionCommand"], str(PACKAGE_JSON), "M314-C002-PKG-02", "package introspection command drifted", failures)
    checks_passed += require(workflow_api.get("contractId") == "objc3c-cleanup-task-runner-workflow-api/m314-c001-v1", str(PACKAGE_JSON), "M314-C002-PKG-03", "package workflow contract should preserve C001 contract id", failures)
    checks_passed += require('"check:objc3c:m314-c002-parameterized-task-runner-implementation-core-feature-implementation"' in package_text and '"check:objc3c:m314-c002-lane-c-readiness"' in package_text, str(PACKAGE_JSON), "M314-C002-PKG-04", "package missing issue-local scripts", failures)
    checks_passed += require(sorted(entry["action"] for entry in live_list["actions"]) == sorted([entry["action"] for entry in live_list["actions"]]), str(RUNNER), "M314-C002-LIVE-06", "live action list malformed", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": registry["mode"],
        "contract_id": registry["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "live_action_count": live_list["action_count"],
        "live_pass_through_actions": [entry["action"] for entry in live_list["actions"] if entry.get("pass_through_args")],
        "next_issue": "M314-C003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-C002 parameterized task runner checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
