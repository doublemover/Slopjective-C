#!/usr/bin/env python3
"""Checker for M318-C002 budget enforcement and regression alarms implementation."""

from __future__ import annotations

import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_budget_enforcement_and_regression_alarms_implementation_core_feature_implementation_c002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_c002_budget_enforcement_and_regression_alarms_implementation_core_feature_implementation_packet.md"
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_c002_budget_enforcement_and_regression_alarms_implementation_core_feature_implementation_result.json"
RUNNER = ROOT / "scripts" / "m318_governance_guard.py"
WORKFLOW = ROOT / ".github" / "workflows" / "m318-governance-budget-enforcement.yml"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-C002" / "budget_enforcement_summary.json"


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> dict:
    return json.loads(read_text(path))


def require(condition: bool, artifact: str, check_id: str, detail: str, failures: list[Finding]) -> int:
    if condition:
        return 1
    failures.append(Finding(artifact, check_id, detail))
    return 0


def main(argv: Sequence[str]) -> int:
    del argv
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    result = read_json(RESULT_JSON)
    workflow = read_text(WORKFLOW)

    checks_total += 4
    checks_passed += require("objc3c-governance-budget-enforcement-implementation/m318-c002-v1" in expectations, str(EXPECTATIONS_DOC), "M318-C002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("shared runner" in expectations.lower() and "ci workflow" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-C002-EXP-02", "expectations missing shared runner/workflow note", failures)
    checks_passed += require("Implemented stages" in packet and "topology" in packet, str(PACKET_DOC), "M318-C002-PKT-01", "packet missing stages section", failures)
    checks_passed += require("Next issue: `M318-C003`." in packet, str(PACKET_DOC), "M318-C002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(result.get("mode") == "m318-c002-governance-budget-enforcement-v1", str(RESULT_JSON), "M318-C002-RES-01", "result mode drifted", failures)
    checks_passed += require(result.get("contract_id") == "objc3c-governance-budget-enforcement-implementation/m318-c002-v1", str(RESULT_JSON), "M318-C002-RES-02", "result contract id drifted", failures)
    checks_passed += require(result.get("runner_path") == "scripts/m318_governance_guard.py", str(RESULT_JSON), "M318-C002-RES-03", "runner path drifted", failures)
    checks_passed += require(result.get("workflow_path") == ".github/workflows/m318-governance-budget-enforcement.yml", str(RESULT_JSON), "M318-C002-RES-04", "workflow path drifted", failures)
    checks_passed += require(result.get("stage_order") == ["budget-snapshot", "exception-registry", "residue-proof", "topology"], str(RESULT_JSON), "M318-C002-RES-05", "stage order drifted", failures)
    checks_passed += require(result.get("next_issue") == "M318-C003", str(RESULT_JSON), "M318-C002-RES-06", "next issue drifted", failures)

    checks_total += 3
    checks_passed += require("python scripts/m318_governance_guard.py --stage topology" in workflow, str(WORKFLOW), "M318-C002-WF-01", "workflow missing topology invocation", failures)
    checks_passed += require("m318-governance-budget-enforcement.yml" in workflow, str(WORKFLOW), "M318-C002-WF-02", "workflow self-path missing", failures)
    checks_passed += require("scripts/check_m315_d002_anti_noise_enforcement_implementation_core_feature_implementation.py" in workflow, str(WORKFLOW), "M318-C002-WF-03", "workflow missing source-hygiene dependency path", failures)

    completed = subprocess.run([sys.executable, str(RUNNER), "--stage", "topology", "--summary-out", str(SUMMARY_PATH)], cwd=ROOT, capture_output=True, text=True, check=False)
    checks_total += 2
    checks_passed += require(completed.returncode == 0, str(RUNNER), "M318-C002-RUN-01", f"topology runner failed: {(completed.stderr or completed.stdout).strip()}", failures)
    payload = read_json(SUMMARY_PATH) if SUMMARY_PATH.exists() else {}
    checks_passed += require(payload.get("ok") is True and payload.get("alarms") == [], str(SUMMARY_PATH), "M318-C002-RUN-02", "topology summary is not green", failures)

    summary = {
        "contract_id": result["contract_id"],
        "runner_path": result["runner_path"],
        "workflow_path": result["workflow_path"],
        "topology_ok": payload.get("ok"),
        "alarms": payload.get("alarms", []),
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [f.__dict__ for f in failures],
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M318-C002 governance guard implementation checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
