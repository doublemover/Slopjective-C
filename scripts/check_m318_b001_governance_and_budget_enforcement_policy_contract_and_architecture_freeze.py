#!/usr/bin/env python3
"""Checker for M318-B001 governance and budget enforcement policy."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_governance_and_budget_enforcement_policy_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_b001_governance_and_budget_enforcement_policy_contract_and_architecture_freeze_packet.md"
POLICY_JSON = ROOT / "spec" / "governance" / "objc3c_anti_noise_budget_policy.json"
A001_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_inventory.json"
A002_JSON = ROOT / "spec" / "governance" / "objc3c_anti_noise_exception_process.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-B001" / "governance_budget_policy_summary.json"


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
    policy = read_json(POLICY_JSON)
    a001 = read_json(A001_JSON)
    a002 = read_json(A002_JSON)

    checks_total += 4
    checks_passed += require("objc3c-governance-anti-noise-budget-policy/m318-b001-v1" in expectations, str(EXPECTATIONS_DOC), "M318-B001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("prohibit, without an approved exception record" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-B001-EXP-02", "expectations missing prohibition note", failures)
    checks_passed += require("Allowed without exception" in packet and "Prohibited without exception" in packet, str(PACKET_DOC), "M318-B001-PKT-01", "packet missing allow/prohibit split", failures)
    checks_passed += require("Next issue: `M318-B002`." in packet, str(PACKET_DOC), "M318-B001-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(policy.get("mode") == "m318-b001-anti-noise-budget-policy-v1", str(POLICY_JSON), "M318-B001-POL-01", "policy mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-governance-anti-noise-budget-policy/m318-b001-v1", str(POLICY_JSON), "M318-B001-POL-02", "policy contract id drifted", failures)
    checks_passed += require(policy.get("consumed_contracts", {}).get("budget_map") == a001.get("contract_id"), str(POLICY_JSON), "M318-B001-POL-03", "budget-map contract link drifted", failures)
    checks_passed += require(policy.get("consumed_contracts", {}).get("exception_process") == a002.get("contract_id"), str(POLICY_JSON), "M318-B001-POL-04", "exception-process contract link drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M318-B002", str(POLICY_JSON), "M318-B001-POL-05", "next issue drifted", failures)

    expected_allowed = [
        "acceptance-suite input growth under tests/tooling/runtime and tests/tooling/fixtures/native",
        "governance docs and runbooks under spec/governance and docs/runbooks",
        "transient planning and publish artifacts under tmp/",
    ]
    expected_prohibited = [
        "public command growth beyond the M314 command-surface budget",
        "new scripts/check_m*_*.py, run_*_readiness.py, or tests/tooling/test_check_*.py families beyond the M313 validation budget",
        "new milestone-coded product identifiers in product code",
        "new synthetic .ll fixtures outside tests/tooling/fixtures/native/library_cli_parity",
        "new replay or proof artifacts without provenance and regeneration metadata",
    ]
    checks_total += 4
    checks_passed += require(policy.get("allowed_without_exception") == expected_allowed, str(POLICY_JSON), "M318-B001-POL-06", "allowed-without-exception list drifted", failures)
    checks_passed += require(policy.get("prohibited_without_exception") == expected_prohibited, str(POLICY_JSON), "M318-B001-POL-07", "prohibited-without-exception list drifted", failures)
    checks_passed += require(policy.get("exception_defaults", {}).get("expired_exception_is_blocking") is True, str(POLICY_JSON), "M318-B001-POL-08", "expired exception policy drifted", failures)
    checks_passed += require(policy.get("exception_defaults", {}).get("registry_must_default_empty") is True, str(POLICY_JSON), "M318-B001-POL-09", "default-empty registry rule drifted", failures)

    ownership = policy.get("budget_family_ownership", {})
    checks_total += 4
    checks_passed += require(ownership.get("public_command_surface", {}).get("existing_owner_issue") == a001["budget_families"]["public_command_surface"]["owner_issue"] == "M314-C003", str(POLICY_JSON), "M318-B001-OWN-01", "public command owner drifted", failures)
    checks_passed += require(ownership.get("validation_growth", {}).get("existing_owner_issue") == a001["budget_families"]["validation_growth"]["policy_owner_issue"] == "M313-B005", str(POLICY_JSON), "M318-B001-OWN-02", "validation owner drifted", failures)
    checks_passed += require(ownership.get("source_hygiene_and_residue", {}).get("existing_owner_issue") == a001["budget_families"]["source_hygiene_and_residue"]["owner_issue"] == "M315-D002", str(POLICY_JSON), "M318-B001-OWN-03", "source-hygiene owner drifted", failures)
    checks_passed += require(ownership.get("artifact_authenticity_and_synthetic_fixtures", {}).get("future_enforcement_issue") == "M318-D001", str(POLICY_JSON), "M318-B001-OWN-04", "artifact-authenticity future owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "contract_id": policy["contract_id"],
        "allowed_without_exception": policy["allowed_without_exception"],
        "prohibited_without_exception": policy["prohibited_without_exception"],
        "budget_family_ownership": ownership,
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
    print(f"[ok] M318-B001 governance budget policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
