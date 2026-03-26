#!/usr/bin/env python3
"""Checker for M313-C002 subsystem executable acceptance suites."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-C002" / "subsystem_executable_acceptance_suites_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_subsystem_executable_acceptance_suites_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c002_subsystem_executable_acceptance_suites_core_feature_implementation_packet.md"
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c002_subsystem_executable_acceptance_suites_core_feature_implementation_plan.json"
SCHEMA_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"
HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
PACKAGE_JSON = ROOT / "package.json"


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


def run_json(command: list[str]) -> tuple[int, dict[str, object] | None, str]:
    completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)
    if completed.returncode != 0:
        return completed.returncode, None, completed.stderr or completed.stdout
    try:
        return completed.returncode, json.loads(completed.stdout), ""
    except json.JSONDecodeError as exc:
        return 1, None, str(exc)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    plan = json.loads(read_text(PLAN_JSON))
    schema = json.loads(read_text(SCHEMA_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-subsystem-executable-acceptance-suites/m313-c002-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-C002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Emit `M313-C001` acceptance artifact envelopes for each suite run." in expectations, str(EXPECTATIONS_DOC), "M313-C002-EXP-02", "expectations missing schema-envelope requirement", failures)
    checks_passed += require("shared-harness `run-suite` execution mode" in packet, str(PACKET_DOC), "M313-C002-PKT-01", "packet missing run-suite focus", failures)
    checks_passed += require("Next issue: `M313-C003`." in packet, str(PACKET_DOC), "M313-C002-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(plan.get("mode") == "m313-c002-subsystem-executable-acceptance-suites-v1", str(PLAN_JSON), "M313-C002-CON-01", "mode drifted", failures)
    checks_passed += require(plan.get("contract_id") == "objc3c-cleanup-subsystem-executable-acceptance-suites/m313-c002-v1", str(PLAN_JSON), "M313-C002-CON-02", "contract id drifted", failures)
    checks_passed += require(plan.get("required_harness_modes") == ["list-suites", "show-suite", "check-roots", "run-suite"], str(PLAN_JSON), "M313-C002-CON-03", "required harness modes drifted", failures)
    checks_passed += require(plan.get("default_report_root") == "tmp/reports/m313/acceptance/<suite_id>/summary.json", str(PLAN_JSON), "M313-C002-CON-04", "default report root drifted", failures)
    checks_passed += require(plan.get("next_issue") == "M313-C003", str(PLAN_JSON), "M313-C002-CON-05", "next issue drifted", failures)

    suite_artifacts: list[dict[str, object]] = []
    for index, suite in enumerate(plan["suite_execution_plan"], start=1):
        suite_id = suite["suite_id"]
        summary_path = ROOT / "tmp" / "reports" / "m313" / "acceptance" / suite_id / "summary.json"
        rc, payload, error = run_json([sys.executable, str(HARNESS), "--run-suite", suite_id, "--summary-out", str(summary_path)])
        checks_total += 8
        checks_passed += require(rc == 0, str(HARNESS), f"M313-C002-HAR-{index:02d}", f"run-suite failed for {suite_id}: {error}", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("schema_version") == schema.get("schema_version"), str(HARNESS), f"M313-C002-HAR-{index+10:02d}", f"suite {suite_id} missing schema version", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("contract_id") == schema.get("contract_id"), str(HARNESS), f"M313-C002-HAR-{index+20:02d}", f"suite {suite_id} contract id drifted", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("suite_id") == suite_id, str(HARNESS), f"M313-C002-HAR-{index+30:02d}", f"suite {suite_id} suite_id drifted", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("artifact_class") == suite.get("artifact_class"), str(HARNESS), f"M313-C002-HAR-{index+40:02d}", f"suite {suite_id} artifact class drifted", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("outputs", {}).get("summary_path") == f"tmp/reports/m313/acceptance/{suite_id}/summary.json", str(HARNESS), f"M313-C002-HAR-{index+50:02d}", f"suite {suite_id} summary path drifted", failures)
        measurements = payload.get("measurements", {}) if isinstance(payload, dict) else {}
        case_counts = measurements.get("case_counts", {}) if isinstance(measurements, dict) else {}
        checks_passed += require((not suite.get("requires_objc3_fixture_count")) or case_counts.get("objc3_fixture_count", 0) > 0, str(HARNESS), f"M313-C002-HAR-{index+60:02d}", f"suite {suite_id} missing objc3 fixture coverage", failures)
        checks_passed += require((not suite.get("requires_runtime_probe_count")) or case_counts.get("runtime_probe_count", 0) > 0, str(HARNESS), f"M313-C002-HAR-{index+70:02d}", f"suite {suite_id} missing runtime probe coverage", failures)
        suite_artifacts.append(payload or {})

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-c002-subsystem-executable-acceptance-suites-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-C002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-c002-subsystem-executable-acceptance-suites-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-C002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-c002-lane-c-readiness"' in package, str(PACKAGE_JSON), "M313-C002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": plan["mode"],
        "contract_id": plan["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "suite_ids": [entry["suite_id"] for entry in plan["suite_execution_plan"]],
        "artifact_classes": [entry.get("artifact_class") for entry in suite_artifacts],
        "next_issue": "M313-C003",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-C002 executable acceptance suite checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
