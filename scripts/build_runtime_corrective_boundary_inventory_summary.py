#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
import json
from pathlib import Path
from typing import Any
from objc3c_evidence_owner_contracts import (
    owner_contract_count,
    owner_contract_ids,
    validate_boundary_owner_contracts,
)

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/boundary_inventory.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/boundary-inventory"
JSON_OUT = OUT_DIR / "runtime_corrective_boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "runtime_corrective_boundary_inventory_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
IR_PATH = ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp"
LOWERING_CONTRACT_PATH = ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h"
RUNTIME_SOURCE_ROOT = ROOT / "native/objc3c/src/runtime"
IR_SOURCE_ROOT = ROOT / "native/objc3c/src/ir"
LOWERING_SOURCE_ROOT = ROOT / "native/objc3c/src/lower"
SOURCE_SUFFIXES = {".cpp", ".h", ".inc"}


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(path: Path, needle: str) -> int:
    return path.read_text(encoding="utf-8").count(needle)


def read_source_surface(paths: list[Path]) -> str:
    seen: set[Path] = set()
    chunks: list[str] = []
    for path in paths:
        candidates = [path]
        if path.is_dir():
            candidates = sorted(
                candidate
                for candidate in path.rglob("*")
                if candidate.is_file() and candidate.suffix in SOURCE_SUFFIXES
            )
        for candidate in candidates:
            resolved = candidate.resolve()
            if resolved in seen or not candidate.is_file():
                continue
            seen.add(resolved)
            chunks.append(candidate.read_text(encoding="utf-8"))
    return "\n".join(chunks)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = read_source_surface([RUNTIME_SOURCE_ROOT])
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    ir_text = read_source_surface([IR_SOURCE_ROOT])
    lowering_contract_text = read_source_surface([LOWERING_SOURCE_ROOT])

    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    claim_surfaces = [ROOT / path for path in contract["claim_surfaces"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_implementation_anchor"] == "scripts/build_runtime_corrective_boundary_inventory_summary.py",
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
        "docs_publish_dispatch_fallback_gap": "unresolved dispatch still has one strict dispatch error path after slow-path miss" in doc_text,
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
        "successor_map_starts_after_corrective_tranche": contract["successor_map"][0]["reason"].startswith("governance ratchet"),
        "non_goals_keep_full_closure_out_of_scope": "no-full-object-model-closure" in contract["explicit_non_goals"],
    }
    checks.update(validate_boundary_owner_contracts("runtime_corrective", contract, ROOT))

    measured_inventory = {
        "focus_track_count": len(contract["focus"]),
        "source_owner_contract_count": owner_contract_count("runtime_corrective"),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "authoritative_probe_count": len(contract["authoritative_probe_paths"]),
        "authoritative_fixture_count": len(contract["authoritative_fixture_paths"]),
        "claim_surface_count": len(contract["claim_surfaces"]),
        "current_gap_count": len(contract["current_corrective_gaps"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_milestone_count": len(contract["successor_map"]),
        "runtime_dispatch_symbol_occurrences": runtime_text.count("objc3_runtime_dispatch_i32"),
        "property_context_binding_occurrences": runtime_text.count("objc3_runtime_bind_current_property_context_for_testing"),
        "weak_current_property_helper_occurrences": runtime_text.count("weak_current_property"),
        "current_property_helper_occurrences": runtime_text.count("current_property"),
        "corrective_gap_claim_occurrences_in_docs": sum(
            count_literal(DOC_PATH, needle)
            for needle in (
                "unresolved dispatch still has one strict dispatch error path after slow-path miss",
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
        "source_owner_contract_ids": list(owner_contract_ids("runtime_corrective")),
        "hard_cutover_source_owner_contract": contract["hard_cutover_source_owner_contract"],
        "current_gap_ids": [gap["gap_id"] for gap in contract["current_corrective_gaps"]],
        "successor_milestones": [entry["milestone"] for entry in contract["successor_map"]],
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, summary)
    MD_OUT.write_text(
        "# Runtime Corrective Boundary Inventory Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Focus tracks: `{measured_inventory['focus_track_count']}`\n"
        f"- Authoritative code paths: `{measured_inventory['authoritative_code_path_count']}`\n"
        f"- Authoritative probes: `{measured_inventory['authoritative_probe_count']}`\n"
        f"- Authoritative fixtures: `{measured_inventory['authoritative_fixture_count']}`\n"
        f"- Source owners: `{measured_inventory['source_owner_contract_count']}`\n"
        f"- Current gaps: `{', '.join(summary['current_gap_ids'])}`\n"
        f"- Successor milestones: `{', '.join(summary['successor_milestones'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
