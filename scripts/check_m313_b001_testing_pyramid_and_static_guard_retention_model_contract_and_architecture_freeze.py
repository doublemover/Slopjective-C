#!/usr/bin/env python3
"""Checker for M313-B001 testing pyramid and static-guard model."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-B001" / "testing_pyramid_static_guard_model_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_b001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_packet.md"
MODEL_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_model.json"
A001_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json"
A002_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
A003_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json"
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
    model = json.loads(read_text(MODEL_JSON))
    a001 = json.loads(read_text(A001_JSON))
    a002 = json.loads(read_text(A002_JSON))
    a003 = json.loads(read_text(A003_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-testing-pyramid-static-guard-model/m313-b001-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-B001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Prefer executable subsystem suites as the primary truth surface." in expectations, str(EXPECTATIONS_DOC), "M313-B001-EXP-02", "expectations missing executable-suite preference", failures)
    checks_passed += require("retained static-guard classes" in packet, str(PACKET_DOC), "M313-B001-PKT-01", "packet missing retained-guard focus", failures)
    checks_passed += require("Next issue: `M313-B002`." in packet, str(PACKET_DOC), "M313-B001-PKT-02", "packet missing next issue", failures)

    checks_total += 9
    checks_passed += require(model.get("mode") == "m313-b001-testing-pyramid-static-guard-model-v1", str(MODEL_JSON), "M313-B001-CON-01", "mode drifted", failures)
    checks_passed += require(model.get("contract_id") == "objc3c-cleanup-testing-pyramid-static-guard-model/m313-b001-v1", str(MODEL_JSON), "M313-B001-CON-02", "contract id drifted", failures)
    checks_passed += require(model.get("baseline_sources") == [
        "spec/planning/compiler/m313/m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json",
        "spec/planning/compiler/m313/m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json",
        "spec/planning/compiler/m313/m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json",
    ], str(MODEL_JSON), "M313-B001-CON-03", "baseline sources drifted", failures)
    checks_passed += require([entry.get("layer") for entry in model.get("pyramid_layers", [])] == ["executable_acceptance_suites", "shared_integration_harnesses", "retained_static_guards", "migration_only_wrappers"], str(MODEL_JSON), "M313-B001-CON-04", "pyramid layers drifted", failures)
    checks_passed += require(model.get("retained_static_guard_classes") == ["schema_contract", "inventory_contract", "publication_consistency", "policy_budget_enforcement"], str(MODEL_JSON), "M313-B001-CON-05", "retained static guard classes drifted", failures)
    checks_passed += require(model.get("migration_only_classes") == ["issue_local_checker", "issue_local_readiness_runner", "issue_local_pytest_wrapper"], str(MODEL_JSON), "M313-B001-CON-06", "migration-only classes drifted", failures)
    checks_passed += require(model.get("prohibited_patterns") == [
        "new milestone-local validation namespaces outside the shared acceptance roots",
        "new one-off readiness chains that bypass the shared harness model",
        "new static guards that duplicate executable suite truth",
    ], str(MODEL_JSON), "M313-B001-CON-07", "prohibited patterns drifted", failures)
    checks_passed += require(model.get("next_issue") == "M313-B002", str(MODEL_JSON), "M313-B001-CON-08", "next issue drifted", failures)
    checks_passed += require([entry.get("truth_priority") for entry in model.get("pyramid_layers", [])] == [1, 2, 3, 4], str(MODEL_JSON), "M313-B001-CON-09", "truth priorities drifted", failures)

    checks_total += 4
    checks_passed += require(a001.get("acceptance_suite_roots") == ["tests/tooling/runtime", "tests/tooling/fixtures/native"], str(A001_JSON), "M313-B001-BASE-01", "A001 acceptance roots drifted", failures)
    checks_passed += require(a002.get("no_new_growth_without_exception") is True, str(A002_JSON), "M313-B001-BASE-02", "A002 no-growth rule drifted", failures)
    checks_passed += require(a003.get("active_acceptance_aggregation", {}).get("roots") == ["tests/tooling/runtime", "tests/tooling/fixtures/native", "tmp/reports"], str(A003_JSON), "M313-B001-BASE-03", "A003 active aggregation roots drifted", failures)
    checks_passed += require(a003.get("migration_owner_sequence", [None])[0] == "M313-B001", str(A003_JSON), "M313-B001-BASE-04", "A003 migration owner sequence no longer starts at M313-B001", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-b001-testing-pyramid-and-static-guard-retention-model-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-B001-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-b001-testing-pyramid-and-static-guard-retention-model-contract-and-architecture-freeze"' in package, str(PACKAGE_JSON), "M313-B001-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-b001-lane-b-readiness"' in package, str(PACKAGE_JSON), "M313-B001-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": model["mode"],
        "contract_id": model["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "pyramid_layers": [entry["layer"] for entry in model["pyramid_layers"]],
        "retained_static_guard_classes": model["retained_static_guard_classes"],
        "migration_only_classes": model["migration_only_classes"],
        "next_issue": "M313-B002",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-B001 testing pyramid model checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
