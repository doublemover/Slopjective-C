#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/boundary_inventory.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/boundary-inventory"
JSON_OUT = OUT_DIR / "runtime_corrective_boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "runtime_corrective_boundary_inventory_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
IR_PATH = ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp"
LOWERING_CONTRACT_PATH = ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(path: Path, needle: str) -> int:
    return path.read_text(encoding="utf-8").count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    ir_text = IR_PATH.read_text(encoding="utf-8")
    lowering_contract_text = LOWERING_CONTRACT_PATH.read_text(encoding="utf-8")

    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    claim_surfaces = [ROOT / path for path in contract["claim_surfaces"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_boundary_inventory_summary.py",
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_claim_surfaces_exist": all(path.is_file() for path in claim_surfaces),
        "runtime_dispatch_symbol_exported": "int objc3_runtime_dispatch_i32(" in runtime_text,
        "property_context_binding_exported": "int objc3_runtime_bind_current_property_context_for_testing(" in runtime_text,
        "current_property_helper_cluster_exported": all(
            needle in runtime_text
            for needle in (
                "objc3_runtime_read_current_property_i32",
                "objc3_runtime_write_current_property_i32",
                "objc3_runtime_exchange_current_property_i32",
                "objc3_runtime_load_weak_current_property_i32",
                "objc3_runtime_store_weak_current_property_i32",
            )
        ),
        "docs_publish_dispatch_fallback_gap": "unresolved dispatch still has one deterministic fallback path after slow-path miss" in doc_text,
        "docs_publish_synthesized_accessor_gap": "synthesized accessor IR still carries transitional lowering residue" in doc_text,
        "docs_publish_native_output_gap": "native-output truth requires the emitted object and linked probe to stay coupled end to end" in doc_text,
        "lowering_contract_contains_current_property_helper_symbols": all(
            needle in ir_text or needle in lowering_contract_text
            for needle in (
                "objc3_runtime_read_current_property_i32",
                "objc3_runtime_write_current_property_i32",
                "objc3_runtime_exchange_current_property_i32",
            )
        ),
        "successor_map_starts_after_corrective_tranche": contract["successor_map"][0]["description"].startswith("governance ratchet"),
        "non_goals_keep_full_closure_out_of_scope": "no-full-object-model-closure" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "focus_track_count": len(contract["focus"]),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "authoritative_probe_count": len(contract["authoritative_probe_paths"]),
        "authoritative_fixture_count": len(contract["authoritative_fixture_paths"]),
        "claim_surface_count": len(contract["claim_surfaces"]),
        "current_gap_count": len(contract["current_corrective_gaps"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_milestone_count": len(contract["successor_map"]),
        "runtime_dispatch_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_dispatch_i32"),
        "property_context_binding_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_bind_current_property_context_for_testing"),
        "weak_current_property_helper_occurrences": count_literal(RUNTIME_PATH, "weak_current_property"),
        "current_property_helper_occurrences": count_literal(RUNTIME_PATH, "current_property"),
        "corrective_gap_claim_occurrences_in_docs": sum(
            count_literal(DOC_PATH, needle)
            for needle in (
                "unresolved dispatch still has one deterministic fallback path after slow-path miss",
                "synthesized accessor IR still carries transitional lowering residue",
                "native-output truth requires the emitted object and linked probe to stay coupled end to end",
            )
        ),
    }

    summary = {
        "issue": "runtime-corrective-boundary-inventory",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "current_gap_ids": [gap["gap_id"] for gap in contract["current_corrective_gaps"]],
        "successor_milestones": [entry["milestone"] for entry in contract["successor_map"]],
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Runtime Corrective Boundary Inventory Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Focus tracks: `{measured_inventory['focus_track_count']}`\n"
        f"- Authoritative code paths: `{measured_inventory['authoritative_code_path_count']}`\n"
        f"- Authoritative probes: `{measured_inventory['authoritative_probe_count']}`\n"
        f"- Authoritative fixtures: `{measured_inventory['authoritative_fixture_count']}`\n"
        f"- Current gaps: `{', '.join(summary['current_gap_ids'])}`\n"
        f"- Successor milestones: `{', '.join(summary['successor_milestones'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
