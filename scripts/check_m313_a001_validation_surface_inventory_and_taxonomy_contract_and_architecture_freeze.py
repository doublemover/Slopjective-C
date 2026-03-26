#!/usr/bin/env python3
"""Checker for M313-A001 validation surface inventory and taxonomy."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-A001" / "validation_surface_inventory_and_taxonomy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json"
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
    inventory = json.loads(read_text(INVENTORY_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-validation-surface-inventory-taxonomy/m313-a001-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-A001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Freeze the measured validation surface" in expectations, str(EXPECTATIONS_DOC), "M313-A001-EXP-02", "expectations missing purpose statement", failures)
    checks_passed += require("measured current validation counts" in packet, str(PACKET_DOC), "M313-A001-PKT-01", "packet missing measured-count focus", failures)
    checks_passed += require("Next issue: `M313-A002`." in packet, str(PACKET_DOC), "M313-A001-PKT-02", "packet missing next issue", failures)

    checks_total += 7
    checks_passed += require(inventory.get("mode") == "m313-a001-validation-surface-inventory-taxonomy-v1", str(INVENTORY_JSON), "M313-A001-CON-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-cleanup-validation-surface-inventory-taxonomy/m313-a001-v1", str(INVENTORY_JSON), "M313-A001-CON-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("acceptance_suite_roots") == ["tests/tooling/runtime", "tests/tooling/fixtures/native"], str(INVENTORY_JSON), "M313-A001-CON-03", "acceptance-suite roots drifted", failures)
    checks_passed += require(inventory.get("namespace_policy", {}).get("active") == ["tests/tooling/runtime", "tests/tooling/fixtures/native", "tmp/reports"], str(INVENTORY_JSON), "M313-A001-CON-04", "active namespace policy drifted", failures)
    checks_passed += require(inventory.get("namespace_policy", {}).get("migration_only") == ["scripts/check_*.py", "scripts/run_*_readiness.py", "tests/tooling/test_check_*.py"], str(INVENTORY_JSON), "M313-A001-CON-05", "migration-only namespace policy drifted", failures)
    checks_passed += require(len(inventory.get("taxonomy", [])) == 6, str(INVENTORY_JSON), "M313-A001-CON-06", "taxonomy entry count drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M313-A002", str(INVENTORY_JSON), "M313-A001-CON-07", "next issue drifted", failures)

    measured = {
        "check_scripts": len(list((ROOT / "scripts").glob("check_*.py"))),
        "readiness_runners": len(list((ROOT / "scripts").glob("run_*_readiness.py"))),
        "pytest_check_files": len(list((ROOT / "tests" / "tooling").glob("test_check_*.py"))),
        "runtime_probe_cpp": len(list((ROOT / "tests" / "tooling" / "runtime").glob("*.cpp"))),
        "native_fixture_objc3": len(list((ROOT / "tests" / "tooling" / "fixtures" / "native").rglob("*.objc3"))),
    }
    for key, expected in inventory.get("measured_counts", {}).items():
        checks_total += 1
        checks_passed += require(measured.get(key) == expected, str(INVENTORY_JSON), f"M313-A001-MEASURE-{key}", f"measured count drifted for {key}: expected {expected}, got {measured.get(key)}", failures)

    taxonomy_names = {entry.get("name") for entry in inventory.get("taxonomy", [])}
    checks_total += 1
    checks_passed += require(taxonomy_names == {"static_policy_checker", "readiness_runner", "issue_local_pytest_wrapper", "runtime_probe", "native_fixture_input", "acceptance_suite_root"}, str(INVENTORY_JSON), "M313-A001-TAX-01", "taxonomy names drifted", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-a001-validation-surface-inventory-and-taxonomy-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-A001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-a001-validation-surface-inventory-and-taxonomy-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-A001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-a001-lane-a-readiness"' in package, str(PACKAGE_JSON), "M313-A001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "measured_counts": measured,
        "taxonomy_names": sorted(taxonomy_names),
        "next_issue": "M313-A002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-A001 validation surface inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
