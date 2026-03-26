#!/usr/bin/env python3
"""Checker for M314-A001 command-surface inventory and script-budget policy."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m314" / "M314-A001" / "command_surface_inventory_budget_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m314_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a001_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a001_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_inventory.json"
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
    package = json.loads(read_text(PACKAGE_JSON))
    scripts = package.get("scripts", {})

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-command-surface-inventory-budget/m314-a001-v1`" in expectations, str(EXPECTATIONS_DOC), "M314-A001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("at most `25` documented package entrypoints" in expectations, str(EXPECTATIONS_DOC), "M314-A001-EXP-02", "expectations missing budget target", failures)
    checks_passed += require("public package entrypoints: `<=25`" in packet, str(PACKET_DOC), "M314-A001-PKT-01", "packet missing script budget", failures)
    checks_passed += require("Next issue: `M314-A002`." in packet, str(PACKET_DOC), "M314-A001-PKT-02", "packet missing next issue", failures)

    checks_total += 10
    checks_passed += require(inventory.get("mode") == "m314-a001-command-surface-inventory-budget-v1", str(INVENTORY_JSON), "M314-A001-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-cleanup-command-surface-inventory-budget/m314-a001-v1", str(INVENTORY_JSON), "M314-A001-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("measured_counts", {}).get("package_scripts") == len(scripts), str(INVENTORY_JSON), "M314-A001-INV-03", f"package script count drifted: expected {inventory.get('measured_counts', {}).get('package_scripts')}, got {len(scripts)}", failures)
    checks_passed += require(inventory.get("measured_counts", {}).get("package_scripts") == 7815, str(INVENTORY_JSON), "M314-A001-INV-04", "frozen package script count drifted", failures)
    checks_passed += require(inventory.get("measured_counts", {}).get("workflow_files") == 9, str(INVENTORY_JSON), "M314-A001-INV-05", "workflow count drifted", failures)
    checks_passed += require(inventory.get("measured_counts", {}).get("prototype_compiler_source_files") == 1, str(INVENTORY_JSON), "M314-A001-INV-06", "prototype compiler count drifted", failures)
    checks_passed += require(inventory.get("public_command_budget", {}).get("maximum_total_public_entrypoints") == 25, str(INVENTORY_JSON), "M314-A001-INV-07", "public command budget drifted", failures)
    checks_passed += require(inventory.get("orchestration_layers", {}).get("public_operator_layer") == "package.json scripts", str(INVENTORY_JSON), "M314-A001-INV-08", "public layer drifted", failures)
    checks_passed += require(inventory.get("orchestration_layers", {}).get("internal_parameterized_layer") == "Python runners under scripts/", str(INVENTORY_JSON), "M314-A001-INV-09", "internal layer drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M314-A002", str(INVENTORY_JSON), "M314-A001-INV-10", "next issue drifted", failures)

    for idx, rel_path in enumerate(inventory.get("workflow_files", []), start=1):
        checks_total += 1
        checks_passed += require((ROOT / rel_path).exists(), rel_path, f"M314-A001-WF-{idx:02d}", "workflow file missing", failures)

    for idx, rel_path in enumerate(inventory.get("prototype_surface", {}).get("source_files", []), start=1):
        checks_total += 1
        checks_passed += require((ROOT / rel_path).exists(), rel_path, f"M314-A001-PROT-{idx:02d}", "prototype source file missing", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m314-a001-command-surface-inventory-and-script-budget-policy-contract-and-architecture-freeze"' in read_text(PACKAGE_JSON), str(PACKAGE_JSON), "M314-A001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m314-a001-command-surface-inventory-and-script-budget-policy-contract-and-architecture-freeze"' in read_text(PACKAGE_JSON), str(PACKAGE_JSON), "M314-A001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m314-a001-lane-a-readiness"' in read_text(PACKAGE_JSON), str(PACKAGE_JSON), "M314-A001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "package_scripts": inventory["measured_counts"]["package_scripts"],
        "workflow_files": inventory["measured_counts"]["workflow_files"],
        "prototype_compiler_source_files": inventory["measured_counts"]["prototype_compiler_source_files"],
        "next_issue": "M314-A002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M314-A001 command-surface inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
