#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/error_runtime_closure/throws_abi_helper_semantics_contract.json"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/throws-abi-helper-semantics"
JSON_OUT = OUT_DIR / "throws_abi_helper_semantics_summary.json"
MD_OUT = OUT_DIR / "throws_abi_helper_semantics_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
SEMA_PATH = ROOT / "native/objc3c/src/sema/objc3_semantic_passes.cpp"
IR_PATH = ROOT / "native/objc3c/src/ir/objc3_ir_emitter.cpp"
WORKFLOW_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
CONFORMANCE_PATH = ROOT / "scripts/check_objc3c_runnable_error_conformance.py"
E2E_PATH = ROOT / "scripts/check_objc3c_runnable_error_end_to_end.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    sema_text = SEMA_PATH.read_text(encoding="utf-8")
    ir_text = IR_PATH.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_PATH.read_text(encoding="utf-8")
    conformance_text = CONFORMANCE_PATH.read_text(encoding="utf-8")
    e2e_text = E2E_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_error_runtime_closure_throws_abi_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "docs_publish_error_runtime_abi_surface": "## Error Runtime ABI And Cleanup Surface" in doc_text,
        "docs_publish_error_runtime_implementation_surface": "## Error Propagation Catch And Cleanup Runtime Implementation Surface" in doc_text,
        "runbook_mentions_private_helper_abi_boundary": "private runtime-owned helper ABI" in runbook_text,
        "runbook_mentions_public_workflow_boundary": "validate-error-conformance" in runbook_text and "validate-runnable-error" in runbook_text,
        "runtime_exports_helper_cluster": all(symbol in runtime_text for symbol in contract["helper_symbols"]),
        "sema_publishes_throw_and_cleanup_profiles": "throws_propagation_summary" in sema_text and "unwind_cleanup_summary" in sema_text,
        "ir_publishes_error_runtime_helper_anchor": "error_handling_error_runtime_bridge_helper" in ir_text and "frontend_objc_throws_propagation_lowering_profile" in ir_text,
        "workflow_preserves_public_error_actions": "def action_validate_error_conformance" in workflow_text and "def action_validate_runnable_error" in workflow_text,
        "conformance_checks_required_error_cases": all(case_id in conformance_text for case_id in contract["authoritative_case_ids"]),
        "e2e_preserves_packaged_error_surface": '"error_runtime_fixture"' in e2e_text and '"error_runtime_probe"' in e2e_text,
    }

    summary = {
        "issue": "error-runtime-closure-throws-abi-helper-semantics",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "helper_symbol_count": len(contract["helper_symbols"]),
        "abi_rule_count": len(contract["abi_rules"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Throws ABI Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Helper symbols: `{summary['helper_symbol_count']}`\n"
        f"- ABI rules: `{summary['abi_rule_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
