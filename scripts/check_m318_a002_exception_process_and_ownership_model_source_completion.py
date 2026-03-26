#!/usr/bin/env python3
"""Checker for M318-A002 exception process and ownership model."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_exception_process_and_ownership_model_source_completion_a002_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_a002_exception_process_and_ownership_model_source_completion_packet.md"
PROCESS_JSON = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_process.json"
REGISTRY_JSON = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_registry.json"
BUDGET_MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-A002" / "exception_process_summary.json"


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
    process = read_json(PROCESS_JSON)
    registry = read_json(REGISTRY_JSON)
    budget_map = read_json(BUDGET_MAP_JSON)
    package = read_json(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-anti-noise-exception-process/m318-a002-v1" in expectations, str(EXPECTATIONS_DOC), "M318-A002-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("keep exceptions empty by default" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-A002-EXP-02", "expectations missing empty-registry note", failures)
    checks_passed += require("allowed statuses" in packet.lower(), str(PACKET_DOC), "M318-A002-PKT-01", "packet missing allowed-status note", failures)
    checks_passed += require("Next issue: `M318-B001`." in packet, str(PACKET_DOC), "M318-A002-PKT-02", "packet missing next issue", failures)

    checks_total += 6
    checks_passed += require(process.get("mode") == "m318-a002-anti-noise-exception-process-v1", str(PROCESS_JSON), "M318-A002-PRO-01", "process mode drifted", failures)
    checks_passed += require(process.get("contract_id") == "objc3c-governance-anti-noise-exception-process/m318-a002-v1", str(PROCESS_JSON), "M318-A002-PRO-02", "process contract id drifted", failures)
    checks_passed += require(process.get("budget_map_contract_id") == budget_map.get("contract_id"), str(PROCESS_JSON), "M318-A002-PRO-03", "budget map contract link drifted", failures)
    checks_passed += require(process.get("governance_owner_issue") == "M318-A002", str(PROCESS_JSON), "M318-A002-PRO-04", "governance owner drifted", failures)
    checks_passed += require(process.get("registry_path") == "spec/governance/objc3c_anti_noise_exception_registry.json", str(PROCESS_JSON), "M318-A002-PRO-05", "registry path drifted", failures)
    checks_passed += require(process.get("next_issue") == "M318-B001", str(PROCESS_JSON), "M318-A002-PRO-06", "next issue drifted", failures)

    expected_budgets = [
        "public_command_surface",
        "validation_growth",
        "source_hygiene_and_residue",
        "artifact_authenticity_and_synthetic_fixtures",
    ]
    expected_statuses = ["active", "expired", "retired", "rejected"]
    expected_fields = [
        "id",
        "budget_family",
        "surface_family",
        "owner_issue",
        "created_by_issue",
        "rationale",
        "approval_issue",
        "expiry_issue",
        "review_issue",
        "requested_delta",
        "rollback_condition",
        "replacement_target",
        "status",
    ]
    checks_total += 5
    checks_passed += require(process.get("budget_families") == expected_budgets, str(PROCESS_JSON), "M318-A002-PRO-07", "budget family list drifted", failures)
    checks_passed += require(process.get("allowed_statuses") == expected_statuses, str(PROCESS_JSON), "M318-A002-PRO-08", "allowed statuses drifted", failures)
    checks_passed += require(process.get("required_record_fields") == expected_fields, str(PROCESS_JSON), "M318-A002-PRO-09", "required record fields drifted", failures)
    checks_passed += require(process.get("record_rules", {}).get("expired_records_are_blocking") is True, str(PROCESS_JSON), "M318-A002-PRO-10", "expired record rule drifted", failures)
    checks_passed += require(process.get("record_rules", {}).get("approval_issue_must_differ_from_created_by_issue") is True, str(PROCESS_JSON), "M318-A002-PRO-11", "approval issue rule drifted", failures)

    checks_total += 4
    checks_passed += require(registry.get("mode") == "m318-a002-anti-noise-exception-registry-v1", str(REGISTRY_JSON), "M318-A002-REG-01", "registry mode drifted", failures)
    checks_passed += require(registry.get("contract_id") == "objc3c-governance-anti-noise-exception-registry/m318-a002-v1", str(REGISTRY_JSON), "M318-A002-REG-02", "registry contract id drifted", failures)
    checks_passed += require(registry.get("process_contract_id") == process.get("contract_id"), str(REGISTRY_JSON), "M318-A002-REG-03", "registry process link drifted", failures)
    checks_passed += require(registry.get("exceptions") == [], str(REGISTRY_JSON), "M318-A002-REG-04", "registry must remain empty by default", failures)

    governance = package.get("objc3cGovernance", {})
    checks_total += 5
    checks_passed += require(governance.get("contractId") == process.get("contract_id"), str(PACKAGE_JSON), "M318-A002-PKG-01", "package governance contract id drifted", failures)
    checks_passed += require(governance.get("antiNoiseBudgetMapPath") == "spec/planning/compiler/m318/m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_inventory.json", str(PACKAGE_JSON), "M318-A002-PKG-02", "package budget map path drifted", failures)
    checks_passed += require(governance.get("antiNoiseExceptionProcessPath") == "spec/governance/objc3c_anti_noise_exception_process.json", str(PACKAGE_JSON), "M318-A002-PKG-03", "package process path drifted", failures)
    checks_passed += require(governance.get("antiNoiseExceptionRegistryPath") == "spec/governance/objc3c_anti_noise_exception_registry.json", str(PACKAGE_JSON), "M318-A002-PKG-04", "package registry path drifted", failures)
    checks_passed += require(governance.get("governanceOwnerIssue") == "M318-A002", str(PACKAGE_JSON), "M318-A002-PKG-05", "package governance owner drifted", failures)

    checks_total += 1
    checks_passed += require(budget_map.get("budget_families", {}).get("exception_process_transition", {}).get("future_process_owner_issue") == "M318-A002", str(BUDGET_MAP_JSON), "M318-A002-LNK-01", "A001 future process owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "process_contract_id": process["contract_id"],
        "registry_contract_id": registry["contract_id"],
        "required_record_fields": process["required_record_fields"],
        "allowed_statuses": process["allowed_statuses"],
        "active_exception_count": len(registry["exceptions"]),
        "package_governance": governance,
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
    print(f"[ok] M318-A002 exception process checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
