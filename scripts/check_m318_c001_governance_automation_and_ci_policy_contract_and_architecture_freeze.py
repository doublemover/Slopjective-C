#!/usr/bin/env python3
"""Checker for M318-C001 governance automation and CI policy contract."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_governance_automation_and_ci_policy_contract_and_architecture_freeze_c001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_c001_governance_automation_and_ci_policy_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "governance" / "objc3c_governance_automation_contract.json"
B001_JSON = ROOT / "spec" / "governance" / "objc3c_anti_noise_budget_policy.json"
B002_JSON = ROOT / "spec" / "governance" / "objc3c_planning_hygiene_policy.json"
B003_JSON = ROOT / "spec" / "governance" / "objc3c_review_and_regression_model.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-C001" / "governance_automation_contract_summary.json"


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
    contract = read_json(CONTRACT_JSON)
    b001 = read_json(B001_JSON)
    b002 = read_json(B002_JSON)
    b003 = read_json(B003_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-automation-contract/m318-c001-v1" in expectations, str(EXPECTATIONS_DOC), "M318-C001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("one shared governance guard runner" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-C001-EXP-02", "expectations missing runner note", failures)
    checks_passed += require("stage order" in packet.lower() and "alarm classes" in packet.lower(), str(PACKET_DOC), "M318-C001-PKT-01", "packet missing stage/alarm sections", failures)
    checks_passed += require("Next issue: `M318-C002`." in packet, str(PACKET_DOC), "M318-C001-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(contract.get("mode") == "m318-c001-governance-automation-contract-v1", str(CONTRACT_JSON), "M318-C001-CON-01", "contract mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-governance-automation-contract/m318-c001-v1", str(CONTRACT_JSON), "M318-C001-CON-02", "contract id drifted", failures)
    checks_passed += require(contract.get("consumed_contracts", {}).get("budget_policy") == b001.get("contract_id"), str(CONTRACT_JSON), "M318-C001-CON-03", "budget policy link drifted", failures)
    checks_passed += require(contract.get("consumed_contracts", {}).get("planning_hygiene") == b002.get("contract_id"), str(CONTRACT_JSON), "M318-C001-CON-04", "planning hygiene link drifted", failures)
    checks_passed += require(contract.get("consumed_contracts", {}).get("review_regression") == b003.get("contract_id"), str(CONTRACT_JSON), "M318-C001-CON-05", "review regression link drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M318-C002", str(CONTRACT_JSON), "M318-C001-CON-06", "next issue drifted", failures)

    checks_total += 5
    checks_passed += require(contract.get("runner_path") == "scripts/m318_governance_guard.py", str(CONTRACT_JSON), "M318-C001-CON-07", "runner path drifted", failures)
    checks_passed += require(contract.get("workflow_path") == ".github/workflows/m318-governance-budget-enforcement.yml", str(CONTRACT_JSON), "M318-C001-CON-08", "workflow path drifted", failures)
    checks_passed += require(contract.get("stage_order") == ["budget-snapshot", "exception-registry", "residue-proof", "topology"], str(CONTRACT_JSON), "M318-C001-CON-09", "stage order drifted", failures)
    checks_passed += require(contract.get("alarm_classes") == ["public_command_budget_regression", "validation_budget_regression", "expired_exception_record", "source_hygiene_regression", "artifact_authenticity_regression"], str(CONTRACT_JSON), "M318-C001-CON-10", "alarm classes drifted", failures)
    checks_passed += require(contract.get("future_tooling_surface", {}).get("owner_issue") == "M318-C003", str(CONTRACT_JSON), "M318-C001-CON-11", "future tooling owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": contract["contract_id"],
        "runner_path": contract["runner_path"],
        "workflow_path": contract["workflow_path"],
        "stage_order": contract["stage_order"],
        "alarm_classes": contract["alarm_classes"],
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
    print(f"[ok] M318-C001 governance automation contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
