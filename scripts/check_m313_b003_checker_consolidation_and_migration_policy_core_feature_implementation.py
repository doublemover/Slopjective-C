#!/usr/bin/env python3
"""Checker for M313-B003 checker consolidation and migration policy."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m313" / "M313-B003" / "checker_consolidation_migration_policy_summary.json"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m313_checker_consolidation_and_migration_policy_core_feature_implementation_b003_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation_packet.md"
POLICY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation_policy.json"
A003_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json"
B001_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_model.json"
B002_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_registry.json"
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
    policy = json.loads(read_text(POLICY_JSON))
    a003 = json.loads(read_text(A003_JSON))
    b001 = json.loads(read_text(B001_JSON))
    b002 = json.loads(read_text(B002_JSON))
    package = read_text(PACKAGE_JSON)

    checks_total += 4
    checks_passed += require("Contract ID: `objc3c-cleanup-checker-consolidation-migration-policy/m313-b003-v1`" in expectations, str(EXPECTATIONS_DOC), "M313-B003-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("Route the major migration-only families onto concrete suite targets" in expectations, str(EXPECTATIONS_DOC), "M313-B003-EXP-02", "expectations missing route requirement", failures)
    checks_passed += require("consolidation routes for the major milestone-local validation families" in packet, str(PACKET_DOC), "M313-B003-PKT-01", "packet missing consolidation-route focus", failures)
    checks_passed += require("Next issue: `M313-B004`." in packet, str(PACKET_DOC), "M313-B003-PKT-02", "packet missing next issue", failures)

    checks_total += 11
    checks_passed += require(policy.get("mode") == "m313-b003-checker-consolidation-migration-policy-v1", str(POLICY_JSON), "M313-B003-CON-01", "mode drifted", failures)
    checks_passed += require(policy.get("contract_id") == "objc3c-cleanup-checker-consolidation-migration-policy/m313-b003-v1", str(POLICY_JSON), "M313-B003-CON-02", "contract id drifted", failures)
    checks_passed += require(policy.get("baseline_sources") == [
        "spec/planning/compiler/m313/m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json",
        "spec/planning/compiler/m313/m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json",
        "spec/planning/compiler/m313/m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json",
        "spec/planning/compiler/m313/m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_model.json",
        "spec/planning/compiler/m313/m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_registry.json",
    ], str(POLICY_JSON), "M313-B003-CON-03", "baseline sources drifted", failures)
    checks_passed += require(policy.get("migration_classes") == b001.get("migration_only_classes"), str(POLICY_JSON), "M313-B003-CON-04", "migration classes no longer align with B001", failures)
    checks_passed += require(policy.get("retained_static_guard_classes") == b001.get("retained_static_guard_classes"), str(POLICY_JSON), "M313-B003-CON-05", "retained static guard classes no longer align with B001", failures)

    routes = policy.get("consolidation_routes", [])
    route_suite_ids = [entry.get("suite_id") for entry in routes]
    registry_suite_ids = [entry.get("suite_id") for entry in b002.get("suite_registry", [])]
    checks_passed += require(route_suite_ids == registry_suite_ids, str(POLICY_JSON), "M313-B003-CON-06", "route suite ids drifted from B002 registry", failures)
    checks_passed += require([entry.get("migration_owner_issue") for entry in routes] == ["M313-C002", "M313-B002", "M313-C002", "M313-B004"], str(POLICY_JSON), "M313-B003-CON-07", "migration owner issues drifted", failures)
    checks_passed += require([entry.get("compatibility_bridge_issue") for entry in routes] == ["M313-C003", "M313-C003", "M313-C003", "M313-E002"], str(POLICY_JSON), "M313-B003-CON-08", "compatibility bridge owners drifted", failures)

    a003_routes = {entry["suite_id"]: entry["migration_only_feeders"] for entry in a003["acceptance_suite_boundaries"]}
    checks_passed += require([entry.get("migration_globs") for entry in routes] == [a003_routes[suite_id] for suite_id in route_suite_ids], str(POLICY_JSON), "M313-B003-CON-09", "route migration globs drifted from A003", failures)
    defaults = policy.get("migration_defaults", {})
    checks_passed += require(defaults.get("new_readiness_must_delegate_to_shared_harness") is True and defaults.get("new_pytest_wrappers_must_wrap_shared_harness_or_retained_static_guard") is True and defaults.get("compatibility_window_closes_by_issue") == "M313-E001", str(POLICY_JSON), "M313-B003-CON-10", "migration defaults drifted", failures)
    checks_passed += require(policy.get("next_issue") == "M313-B004", str(POLICY_JSON), "M313-B003-CON-11", "next issue drifted", failures)

    rc, harness_payload, harness_error = run_json([sys.executable, str(HARNESS), "--list-suites"])
    checks_total += 2
    checks_passed += require(rc == 0, str(HARNESS), "M313-B003-HARNESS-01", f"list-suites failed: {harness_error}", failures)
    checks_passed += require(isinstance(harness_payload, dict) and [entry.get("suite_id") for entry in harness_payload.get("suite_registry", [])] == route_suite_ids, str(HARNESS), "M313-B003-HARNESS-02", "harness suite ids drifted from consolidation routes", failures)

    checks_total += 3
    checks_passed += require('"check:objc3c:m313-b003-checker-consolidation-and-migration-policy-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B003-PKG-01", "package missing checker script", failures)
    checks_passed += require('"test:tooling:m313-b003-checker-consolidation-and-migration-policy-core-feature-implementation"' in package, str(PACKAGE_JSON), "M313-B003-PKG-02", "package missing pytest script", failures)
    checks_passed += require('"check:objc3c:m313-b003-lane-b-readiness"' in package, str(PACKAGE_JSON), "M313-B003-PKG-03", "package missing readiness script", failures)

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": policy["mode"],
        "contract_id": policy["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "route_ids": [entry["route_id"] for entry in routes],
        "suite_ids": route_suite_ids,
        "migration_owner_issues": [entry["migration_owner_issue"] for entry in routes],
        "compatibility_bridge_issues": [entry["compatibility_bridge_issue"] for entry in routes],
        "next_issue": "M313-B004",
        "failures": [finding.__dict__ for finding in failures],
    }
    args.summary_out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] M313-B003 checker consolidation policy checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
