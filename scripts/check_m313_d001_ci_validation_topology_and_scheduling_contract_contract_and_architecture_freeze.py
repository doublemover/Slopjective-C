#!/usr/bin/env python3
"""Checker for M313-D001 CI validation topology and scheduling contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-D001" / "ci_validation_topology_scheduling_contract_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_ci_validation_topology_and_scheduling_contract_contract_and_architecture_freeze_d001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d001_ci_validation_topology_and_scheduling_contract_contract_and_architecture_freeze_packet.md"
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_d001_ci_validation_topology_and_scheduling_contract_contract_and_architecture_freeze_contract.json"
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


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    failures: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    expectations = read_text(EXPECTATIONS_DOC)
    packet = read_text(PACKET_DOC)
    contract = json.loads(read_text(CONTRACT_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-ci-validation-topology-scheduling-contract/m313-d001-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-D001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Freeze one explicit CI validation topology" in expectations, str(EXPECTATIONS_DOC), "M313-D001-EXP-02", "expectations missing topology purpose", failures)
    checks_passed += require("canonical report roots" in packet, str(PACKET_DOC), "M313-D001-PKT-01", "packet missing report-root focus", failures)
    checks_passed += require("Next issue: `M313-D002`." in packet, str(PACKET_DOC), "M313-D001-PKT-02", "packet missing next issue", failures)

    checks_total += 11
    checks_passed += require(contract.get("mode") == "m313-d001-ci-validation-topology-scheduling-contract-v1", str(CONTRACT_JSON), "M313-D001-CON-01", "mode drifted", failures)
    checks_passed += require(contract.get("contract_id") == "objc3c-cleanup-ci-validation-topology-scheduling-contract/m313-d001-v1", str(CONTRACT_JSON), "M313-D001-CON-02", "contract id drifted", failures)
    checks_passed += require(len(contract.get("baseline_sources", [])) == 8, str(CONTRACT_JSON), "M313-D001-CON-03", "baseline source count drifted", failures)
    checks_passed += require(len(contract.get("existing_workflow_references", [])) == 4, str(CONTRACT_JSON), "M313-D001-CON-04", "existing workflow reference count drifted", failures)
    checks_passed += require(contract.get("future_workflow", {}).get("path") == ".github/workflows/m313-validation-acceptance-first.yml", str(CONTRACT_JSON), "M313-D001-CON-05", "future workflow path drifted", failures)
    checks_passed += require([job["job_id"] for job in contract.get("future_workflow", {}).get("jobs", [])] == ["static-policy-guards", "acceptance-suites", "compatibility-bridges", "validation-topology"], str(CONTRACT_JSON), "M313-D001-CON-06", "future workflow jobs drifted", failures)
    checks_passed += require([job["stage"] for job in contract.get("future_workflow", {}).get("jobs", [])] == ["static-guards", "acceptance-suites", "compatibility-bridges", "topology"], str(CONTRACT_JSON), "M313-D001-CON-07", "future workflow stages drifted", failures)
    checks_passed += require(len(contract.get("retained_static_guards", [])) == 5, str(CONTRACT_JSON), "M313-D001-CON-08", "retained static guard set drifted", failures)
    checks_passed += require(contract.get("topology_runner", {}).get("script") == "scripts/m313_acceptance_first_ci_runner.py", str(CONTRACT_JSON), "M313-D001-CON-09", "topology runner script drifted", failures)
    checks_passed += require(contract.get("scheduling_order") == ["retained_static_guards", "acceptance_suites", "compatibility_bridges", "validation_noise_snapshot"], str(CONTRACT_JSON), "M313-D001-CON-10", "scheduling order drifted", failures)
    checks_passed += require(contract.get("next_issue") == "M313-D002", str(CONTRACT_JSON), "M313-D001-CON-11", "next issue drifted", failures)

    for idx, rel_path in enumerate(contract["existing_workflow_references"], start=1):
        checks_total += 1
        checks_passed += require((ROOT / rel_path).exists(), rel_path, f"M313-D001-WF-{idx:02d}", "referenced workflow missing", failures)

    for idx, rel_path in enumerate(contract["baseline_sources"], start=1):
        checks_total += 1
        checks_passed += require((ROOT / rel_path).exists(), rel_path, f"M313-D001-BASE-{idx:02d}", "baseline source missing", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-d001-ci-validation-topology-and-scheduling-contract-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-D001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-d001-ci-validation-topology-and-scheduling-contract-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-D001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-d001-lane-d-readiness"' in package, str(PACKAGE_JSON), "M313-D001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": contract["mode"],
        "contract_id": contract["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "future_workflow_path": contract["future_workflow"]["path"],
        "retained_static_guard_ids": [entry["guard_id"] for entry in contract["retained_static_guards"]],
        "next_issue": "M313-D002",
        "failures": [finding.__dict__ for finding in failures]
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-D001 CI topology contract checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
