#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/object_model_closure/boundary_inventory.json"
OUT_DIR = ROOT / "tmp/reports/m319/M319-A001"
JSON_OUT = OUT_DIR / "object_model_closure_boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "object_model_closure_boundary_inventory_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(path: Path, needle: str) -> int:
    return path.read_text(encoding="utf-8").count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")

    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    claim_surfaces = [ROOT / path for path in contract["claim_surfaces"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_object_model_closure.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_object_model_closure_boundary_inventory_summary.py",
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_claim_surfaces_exist": all(path.is_file() for path in claim_surfaces),
        "runtime_register_image_exported": "int objc3_runtime_register_image(" in runtime_text,
        "runtime_dispatch_symbol_exported": "int objc3_runtime_dispatch_i32(" in runtime_text,
        "realized_class_snapshot_exported": "int objc3_runtime_copy_realized_class_graph_state_for_testing(" in runtime_text,
        "aggregate_object_model_query_exported": "int objc3_runtime_copy_object_model_query_state_for_testing(" in runtime_text,
        "protocol_conformance_query_exported": "int objc3_runtime_copy_protocol_conformance_query_for_testing(" in runtime_text,
        "docs_publish_object_model_abi_query_boundary": "This is the authoritative object-model runtime ABI/query boundary." in doc_text,
        "docs_publish_realization_lookup_boundary": "This is the authoritative live implementation boundary for realization lookup" in doc_text,
        "docs_publish_private_reflection_visibility_constraint": "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state" in doc_text,
        "non_goals_keep_public_abi_narrow": "no-public-runtime-abi-widening" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "focus_track_count": len(contract["focus"]),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "authoritative_probe_count": len(contract["authoritative_probe_paths"]),
        "authoritative_fixture_count": len(contract["authoritative_fixture_paths"]),
        "claim_surface_count": len(contract["claim_surfaces"]),
        "current_gap_count": len(contract["current_closure_gaps"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_milestone_count": len(contract["successor_map"]),
        "realized_class_snapshot_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_copy_realized_class"),
        "property_snapshot_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_copy_property"),
        "protocol_query_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_copy_protocol_conformance_query_for_testing"),
        "object_model_query_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_copy_object_model_query_state_for_testing"),
        "registration_replay_symbol_occurrences": count_literal(RUNTIME_PATH, "objc3_runtime_replay_registered_images_for_testing"),
        "object_model_claim_occurrences_in_docs": sum(
            count_literal(DOC_PATH, needle)
            for needle in (
                "This is the authoritative object-model runtime ABI/query boundary.",
                "This is the authoritative live implementation boundary for realization lookup",
                "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state",
            )
        ),
    }

    summary = {
        "issue": "M319-A001",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "current_gap_ids": [gap["gap_id"] for gap in contract["current_closure_gaps"]],
        "successor_milestones": [entry["milestone"] for entry in contract["successor_map"]],
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M319-A001 Object-Model Closure Boundary Inventory Summary\n\n"
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
