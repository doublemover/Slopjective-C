#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/block_arc_closure/arc_automation_lifetime_insertion_policy.json"
OUT_DIR = ROOT / "tmp/reports/m320/M320-B002"
JSON_OUT = OUT_DIR / "arc_automation_lifetime_insertion_policy_summary.json"
MD_OUT = OUT_DIR / "arc_automation_lifetime_insertion_policy_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_block_arc_closure.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_block_arc_closure_arc_policy_summary.py",
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runtime_exports_all_inserted_lifetime_operations": all(
            f'extern "C" {"void" if symbol.endswith("_scope") or symbol.endswith("_i32") and "store" in symbol else "int"} {symbol}(' in runtime_text
            or f'extern "C" void {symbol}(' in runtime_text
            or f'extern "C" int {symbol}(' in runtime_text
            for symbol in contract["inserted_lifetime_operations"]
        ),
        "docs_publish_block_arc_runtime_abi_surface": "## Block/ARC Runtime ABI Surface" in doc_text,
        "docs_publish_arc_unified_boundary": "This is the authoritative block/ARC unified source boundary." in doc_text,
        "runbook_mentions_arc_ownership_story": "ARC retain/release/autorelease/autoreleasepool and weak/current-property helper traffic as part of one executable ownership story" in runbook_text,
        "runbook_mentions_deferred_error_concurrency_claims": "interaction claims for properties, cleanup, and future error/concurrency surfaces must stay narrower than the evidence published today" in runbook_text,
    }

    summary = {
        "issue": "M320-B002",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "source_surface_contract_count": len(contract["source_surface_contract_ids"]),
        "inserted_lifetime_operation_count": len(contract["inserted_lifetime_operations"]),
        "lowering_surface_path_count": len(contract["lowering_surface_paths"]),
        "policy_model_count": len(contract["policy_models"]),
        "interaction_boundary_count": len(contract["interaction_boundaries"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M320-B002 ARC Automation Lifetime Insertion Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Source surface contracts: `{summary['source_surface_contract_count']}`\n"
        f"- Inserted lifetime operations: `{summary['inserted_lifetime_operation_count']}`\n"
        f"- Lowering surface paths: `{summary['lowering_surface_path_count']}`\n"
        f"- Policy models: `{summary['policy_model_count']}`\n"
        f"- Interaction boundaries: `{summary['interaction_boundary_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
