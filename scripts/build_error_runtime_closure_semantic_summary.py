#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/error_runtime_closure/error_propagation_unwind_cleanup_semantic_model.json"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/error-propagation-unwind-cleanup-semantic"
JSON_OUT = OUT_DIR / "error_propagation_unwind_cleanup_semantic_summary.json"
MD_OUT = OUT_DIR / "error_propagation_unwind_cleanup_semantic_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
SEMA_PATH = ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp"
IR_PATH = ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    sema_text = SEMA_PATH.read_text(encoding="utf-8")
    ir_text = IR_PATH.read_text(encoding="utf-8")
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_error_runtime_closure_semantic_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "docs_publish_error_execution_cleanup_surface": "## Error Execution And Cleanup Source Surface" in doc_text,
        "docs_publish_catch_filter_finalization_surface": "## Catch Filter And Finalization Source Surface" in doc_text,
        "docs_publish_error_cleanup_semantics_surface": "## Error Propagation And Cleanup Semantics Surface" in doc_text,
        "docs_publish_error_runtime_implementation_surface": "## Error Propagation Catch And Cleanup Runtime Implementation Surface" in doc_text,
        "runbook_mentions_runtime_backed_throw_cleanup_model": "throw/catch and cleanup semantics are only supported as runtime-backed behavior" in runbook_text,
        "runbook_mentions_coupled_unwind_cleanup_story": "unwind ordering, cleanup execution, and catch filtering are one coupled runtime story" in runbook_text,
        "sema_exports_throws_propagation_summary": "throws_propagation_summary" in sema_text,
        "sema_exports_unwind_cleanup_summary": "unwind_cleanup_summary" in sema_text,
        "ir_publishes_throws_lowering_profile": "frontend_objc_throws_propagation_lowering_profile" in ir_text,
        "ir_publishes_unwind_cleanup_profile": "frontend_objc_unwind_cleanup_lowering_profile" in ir_text,
        "runtime_exports_thrown_error_store_load": "objc3_runtime_store_thrown_error_i32" in runtime_text and "objc3_runtime_load_thrown_error_i32" in runtime_text,
        "runtime_exports_catch_match_and_bridge_helpers": "objc3_runtime_bridge_status_error_i32" in runtime_text and "objc3_runtime_bridge_nserror_error_i32" in runtime_text and "objc3_runtime_catch_matches_error_i32" in runtime_text,
    }

    summary = {
        "issue": "error-runtime-closure-error-propagation-unwind-cleanup-semantic",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "semantic_surface_path_count": len(contract["semantic_surface_paths"]),
        "semantic_profile_field_count": len(contract["semantic_profile_fields"]),
        "frozen_semantic_model_count": len(contract["frozen_semantic_models"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Semantic Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Semantic surface paths: `{summary['semantic_surface_path_count']}`\n"
        f"- Semantic profile fields: `{summary['semantic_profile_field_count']}`\n"
        f"- Frozen semantic models: `{summary['frozen_semantic_model_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
