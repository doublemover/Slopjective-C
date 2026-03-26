#!/usr/bin/env python3
"""Checker for M313-D002 CI migration to acceptance-first validation."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-D002" / "ci_acceptance_first_validation_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_ci_migration_to_acceptance_first_validation_core_feature_implementation_d002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d002_ci_migration_to_acceptance_first_validation_core_feature_implementation_packet.md"
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d002_ci_migration_to_acceptance_first_validation_core_feature_implementation_plan.json"
RUNNER = ROOT / "scripts" / "m313_acceptance_first_ci_runner.py"
WORKFLOW = ROOT / ".github" / "workflows" / "m313-validation-acceptance-first.yml"
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
    package = read_text(PACKAGE_JSON)
    workflow_text = read_text(WORKFLOW)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-D002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("canonical stage summaries" in expectations, str(EXPECTATIONS_DOC), "M313-D002-EXP-02", "expectations missing stage-summary requirement", failures)
    checks_passed += require("Acceptance-first CI execution" in packet, str(PACKET_DOC), "M313-D002-PKT-01", "packet missing acceptance-first focus", failures)
    checks_passed += require("Next issue: `M313-E001`." in packet, str(PACKET_DOC), "M313-D002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(plan.get("mode") == "m313-d002-ci-acceptance-first-migration-v1", str(PLAN_JSON), "M313-D002-CON-01", "mode drifted", failures)
    checks_passed += require(plan.get("contract_id") == "objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1", str(PLAN_JSON), "M313-D002-CON-02", "contract id drifted", failures)
    checks_passed += require(plan.get("runner_script") == "scripts/m313_acceptance_first_ci_runner.py", str(PLAN_JSON), "M313-D002-CON-03", "runner script drifted", failures)
    checks_passed += require(plan.get("workflow_path") == ".github/workflows/m313-validation-acceptance-first.yml", str(PLAN_JSON), "M313-D002-CON-04", "workflow path drifted", failures)
    checks_passed += require(plan.get("supported_stages") == ["static-guards", "acceptance-suites", "compatibility-bridges", "topology"], str(PLAN_JSON), "M313-D002-CON-05", "supported stages drifted", failures)
    checks_passed += require(plan.get("next_issue") == "M313-E001", str(PLAN_JSON), "M313-D002-CON-06", "next issue drifted", failures)

    stage_payloads: dict[str, dict[str, object]] = {}
    for index, stage in enumerate(plan["supported_stages"], start=1):
        target = ROOT / plan["default_reports"][stage]
        rc, payload, error = run_json([sys.executable, str(RUNNER), "--stage", stage, "--summary-out", str(target)])
        checks_total += 3
        checks_passed += require(rc == 0, str(RUNNER), f"M313-D002-RUN-{index:02d}", f"runner failed for stage {stage}: {error}", failures)
        checks_passed += require(isinstance(payload, dict) and payload.get("stage") == stage, str(RUNNER), f"M313-D002-RUN-{index+10:02d}", f"runner stage drifted for {stage}", failures)
        checks_passed += require(target.exists(), str(target), f"M313-D002-RUN-{index+20:02d}", f"runner did not materialize summary for {stage}", failures)
        if isinstance(payload, dict):
            stage_payloads[stage] = payload

    checks_total += 8
    checks_passed += require("static-policy-guards:" in workflow_text, str(WORKFLOW), "M313-D002-WF-01", "workflow missing static-policy-guards job", failures)
    checks_passed += require("acceptance-suites:" in workflow_text, str(WORKFLOW), "M313-D002-WF-02", "workflow missing acceptance-suites job", failures)
    checks_passed += require("compatibility-bridges:" in workflow_text, str(WORKFLOW), "M313-D002-WF-03", "workflow missing compatibility-bridges job", failures)
    checks_passed += require("validation-topology:" in workflow_text, str(WORKFLOW), "M313-D002-WF-04", "workflow missing validation-topology job", failures)
    checks_passed += require("--stage static-guards" in workflow_text, str(WORKFLOW), "M313-D002-WF-05", "workflow missing static-guards command", failures)
    checks_passed += require("--stage acceptance-suites" in workflow_text, str(WORKFLOW), "M313-D002-WF-06", "workflow missing acceptance-suites command", failures)
    checks_passed += require("--stage compatibility-bridges" in workflow_text, str(WORKFLOW), "M313-D002-WF-07", "workflow missing compatibility-bridges command", failures)
    checks_passed += require("--stage topology" in workflow_text, str(WORKFLOW), "M313-D002-WF-08", "workflow missing topology command", failures)

    topo = stage_payloads.get("topology", {})
    checks_total += 5
    checks_passed += require(topo.get("ok") is True, str(RUNNER), "M313-D002-TOPO-01", "topology stage did not report ok=true", failures)
    checks_passed += require(len(topo.get("suite_results", [])) == 4, str(RUNNER), "M313-D002-TOPO-02", "topology stage did not publish four suite results", failures)
    checks_passed += require(len(topo.get("bridge_results", [])) == 4, str(RUNNER), "M313-D002-TOPO-03", "topology stage did not publish four bridge results", failures)
    checks_passed += require(topo.get("budget_snapshot", {}).get("active_exception_count") == 0, str(RUNNER), "M313-D002-TOPO-04", "topology stage observed active exceptions", failures)
    checks_passed += require(topo.get("budget_snapshot", {}).get("gate_threshold_met") is True, str(RUNNER), "M313-D002-TOPO-05", "topology stage failed gate-threshold snapshot", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-d002-ci-migration-to-acceptance-first-validation-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-D002-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-d002-ci-migration-to-acceptance-first-validation-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-D002-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-d002-lane-d-readiness"' in package, str(PACKAGE_JSON), "M313-D002-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": plan["mode"],
        "contract_id": plan["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "workflow_path": plan["workflow_path"],
        "supported_stages": plan["supported_stages"],
        "topology_summary_path": plan["default_reports"]["topology"],
        "next_issue": "M313-E001",
        "failures": [finding.__dict__ for finding in failures]
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-D002 acceptance-first CI migration checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
