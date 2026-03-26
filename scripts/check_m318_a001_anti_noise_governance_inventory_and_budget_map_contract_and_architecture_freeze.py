#!/usr/bin/env python3
"""Checker for M318-A001 anti-noise governance inventory and budget map."""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m318_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_a001_expectations.md"
PACKET_DOC = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_packet.md"
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m318" / "m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
M313_A002 = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"
M313_B005_POLICY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_policy.json"
M313_B005_REGISTRY = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json"
M315_A003 = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion_inventory.json"
M315_B004 = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_registry.json"
M315_C004 = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_c004_synthetic_fixture_relocation_labeling_and_parity_test_updates_core_feature_expansion_result.json"
M315_D002 = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_d002_anti_noise_enforcement_implementation_core_feature_implementation_result.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "m318" / "M318-A001" / "anti_noise_governance_inventory_summary.json"


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
    inventory = read_json(INVENTORY_JSON)
    package_json = read_json(PACKAGE_JSON)
    m313_a002 = read_json(M313_A002)
    m313_b005_policy = read_json(M313_B005_POLICY)
    m313_b005_registry = read_json(M313_B005_REGISTRY)
    m315_a003 = read_json(M315_A003)
    m315_b004 = read_json(M315_B004)
    m315_c004 = read_json(M315_C004)
    m315_d002 = read_json(M315_D002)

    checks_total += 4
    checks_passed += require("objc3c-governance-anti-noise-budget-map/m318-a001-v1" in expectations, str(EXPECTATIONS_DOC), "M318-A001-EXP-01", "expectations contract id missing", failures)
    checks_passed += require("public command surface budget" in expectations.lower(), str(EXPECTATIONS_DOC), "M318-A001-EXP-02", "expectations missing command-surface note", failures)
    checks_passed += require("validation-growth surface" in packet.lower(), str(PACKET_DOC), "M318-A001-PKT-01", "packet missing validation-growth section", failures)
    checks_passed += require("Next issue: `M318-A002`." in packet, str(PACKET_DOC), "M318-A001-PKT-02", "packet missing next issue", failures)

    checks_total += 5
    checks_passed += require(inventory.get("mode") == "m318-a001-anti-noise-governance-budget-map-v1", str(INVENTORY_JSON), "M318-A001-INV-01", "mode drifted", failures)
    checks_passed += require(inventory.get("contract_id") == "objc3c-governance-anti-noise-budget-map/m318-a001-v1", str(INVENTORY_JSON), "M318-A001-INV-02", "contract id drifted", failures)
    checks_passed += require(inventory.get("next_issue") == "M318-A002", str(INVENTORY_JSON), "M318-A001-INV-03", "next issue drifted", failures)
    checks_passed += require(set(inventory.get("budget_families", {}).keys()) == {"public_command_surface", "validation_growth", "source_hygiene_and_residue", "artifact_authenticity_and_synthetic_fixtures", "exception_process_transition"}, str(INVENTORY_JSON), "M318-A001-INV-04", "budget family set drifted", failures)
    checks_passed += require(inventory.get("baseline_sources", {}).get("command_surface") == "package.json#objc3cCommandSurface", str(INVENTORY_JSON), "M318-A001-INV-05", "command surface baseline source drifted", failures)

    cmd = inventory["budget_families"]["public_command_surface"]
    checks_total += 4
    checks_passed += require(cmd.get("maximum_total_public_entrypoints") == package_json["objc3cCommandSurface"]["budgetMaximum"] == 25, str(INVENTORY_JSON), "M318-A001-CMD-01", "public command budget drifted", failures)
    checks_passed += require(cmd.get("current_public_entrypoints") == package_json["objc3cCommandSurface"]["workflowApi"]["publicActionCount"] == 17, str(INVENTORY_JSON), "M318-A001-CMD-02", "public action count drifted", failures)
    checks_passed += require(cmd.get("owner_issue") == package_json["objc3cCommandSurface"]["budgetEnforcementOwnerIssue"] == "M314-C003", str(INVENTORY_JSON), "M318-A001-CMD-03", "public command owner drifted", failures)
    checks_passed += require(cmd.get("future_enforcement_issue") == "M318-C002", str(INVENTORY_JSON), "M318-A001-CMD-04", "future command enforcement owner drifted", failures)

    val = inventory["budget_families"]["validation_growth"]
    checks_total += 4
    checks_passed += require(val.get("closeout_maximums") == m313_a002.get("closeout_maximums"), str(INVENTORY_JSON), "M318-A001-VAL-01", "validation closeout maximums drifted", failures)
    checks_passed += require(val.get("no_new_growth_without_exception") == m313_a002.get("no_new_growth_without_exception") == True, str(INVENTORY_JSON), "M318-A001-VAL-02", "no-growth policy drifted", failures)
    checks_passed += require(val.get("policy_owner_issue") == "M313-B005", str(INVENTORY_JSON), "M318-A001-VAL-03", "validation policy owner drifted", failures)
    checks_passed += require(m313_b005_policy.get("exception_registry_path") == inventory["baseline_sources"]["validation_exception_registry"], str(INVENTORY_JSON), "M318-A001-VAL-04", "exception registry source drifted", failures)

    hygiene = inventory["budget_families"]["source_hygiene_and_residue"]
    checks_total += 4
    checks_passed += require(hygiene.get("zero_target_classes_removed") == m315_d002.get("zero_target_classes_removed"), str(INVENTORY_JSON), "M318-A001-HYG-01", "zero-target class set drifted", failures)
    checks_passed += require(hygiene.get("remaining_quarantined_residual_classes") == m315_d002.get("remaining_quarantined_residual_classes"), str(INVENTORY_JSON), "M318-A001-HYG-02", "quarantined residual counts drifted", failures)
    checks_passed += require(hygiene.get("post_cleanup_native_source_milestone_token_lines") == m315_d002.get("post_cleanup_native_source_milestone_token_lines") == 57, str(INVENTORY_JSON), "M318-A001-HYG-03", "native source token line count drifted", failures)
    checks_passed += require(hygiene.get("owner_issue") == "M315-D002", str(INVENTORY_JSON), "M318-A001-HYG-04", "source-hygiene owner drifted", failures)

    art = inventory["budget_families"]["artifact_authenticity_and_synthetic_fixtures"]
    checks_total += 5
    checks_passed += require(art.get("synthetic_stub_ir_files") == m315_b004["ll_fixture_totals"]["synthetic_fixture_files"] == 2, str(INVENTORY_JSON), "M318-A001-ART-01", "synthetic stub ll budget drifted", failures)
    checks_passed += require(art.get("generated_replay_candidate_files") == m315_b004["ll_fixture_totals"]["generated_replay_candidate_files"] == 76, str(INVENTORY_JSON), "M318-A001-ART-02", "generated replay candidate count drifted", failures)
    checks_passed += require(art.get("replay_without_frontend_header") == m315_b004["ll_fixture_totals"]["replay_without_frontend_header"] == 46, str(INVENTORY_JSON), "M318-A001-ART-03", "legacy replay-without-header count drifted", failures)
    checks_passed += require(art.get("synthetic_fixture_root") == m315_c004.get("fixture_root") == "tests/tooling/fixtures/native/library_cli_parity", str(INVENTORY_JSON), "M318-A001-ART-04", "synthetic fixture root drifted", failures)
    checks_passed += require(m315_a003["authenticity_class_counts"]["synthetic_fixture"] == 2209, str(M315_A003), "M318-A001-ART-05", "A003 synthetic fixture baseline drifted", failures)

    exn = inventory["budget_families"]["exception_process_transition"]
    checks_total += 3
    checks_passed += require(exn.get("active_exception_count") == len(m313_b005_registry.get("exceptions", [])) == 0, str(INVENTORY_JSON), "M318-A001-EXN-01", "active exception count drifted", failures)
    checks_passed += require(exn.get("current_registry_owner_issue") == "M313-B005", str(INVENTORY_JSON), "M318-A001-EXN-02", "current exception owner drifted", failures)
    checks_passed += require(exn.get("future_process_owner_issue") == "M318-A002", str(INVENTORY_JSON), "M318-A001-EXN-03", "future exception process owner drifted", failures)

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": inventory["mode"],
        "contract_id": inventory["contract_id"],
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "public_command_budget": cmd,
        "validation_growth_budget": val,
        "source_hygiene_budget": hygiene,
        "artifact_authenticity_budget": art,
        "exception_process_transition": exn,
        "failures": [f.__dict__ for f in failures],
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[fail] {finding.artifact} {finding.check_id}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] M318-A001 anti-noise governance inventory checks passed ({checks_passed}/{checks_total})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
