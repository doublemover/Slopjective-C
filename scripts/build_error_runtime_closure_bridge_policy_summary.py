#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/error_runtime_closure/bridged_error_cross_module_compatibility_policy.json"
OUT_DIR = ROOT / "tmp/reports/error-runtime-closure/bridged-error-cross-module-compatibility"
JSON_OUT = OUT_DIR / "bridged_error_cross_module_compatibility_policy_summary.json"
MD_OUT = OUT_DIR / "bridged_error_cross_module_compatibility_policy_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
CONFORMANCE_PATH = ROOT / "scripts/check_objc3c_runnable_error_conformance.py"
E2E_PATH = ROOT / "scripts/check_objc3c_runnable_error_end_to_end.py"
RESULT_BRIDGE_CONSUMER_PATH = ROOT / "tests/tooling/fixtures/native/result_bridge_consumer.objc3"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    conformance_text = CONFORMANCE_PATH.read_text(encoding="utf-8")
    e2e_text = E2E_PATH.read_text(encoding="utf-8")
    result_bridge_text = RESULT_BRIDGE_CONSUMER_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_error_runtime_closure_bridge_policy_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "docs_publish_bridge_diagnostics_surface": "## Bridging Filter And Unwind Diagnostics Surface" in doc_text,
        "docs_publish_error_lowering_bridge_surface": "## Error Lowering Unwind And Bridge Helper Surface" in doc_text,
        "runbook_mentions_cross_module_claim_limit": "cross-module propagation claims are limited to the manifest/runtime-registration/replay path" in runbook_text,
        "runbook_mentions_private_helper_abi_constraint": "private runtime helper ABI" in runbook_text,
        "runtime_exports_bridge_helper_cluster": all(symbol in runtime_text for symbol in contract["bridge_helper_symbols"]),
        "conformance_requires_cross_module_case": "cross-module-error-metadata-replay-preservation" in conformance_text,
        "conformance_requires_runtime_bridge_cases": "bridging-filter-unwind-compatibility-diagnostics" in conformance_text and "error-lowering-unwind-bridge-helper-surface" in conformance_text,
        "e2e_preserves_packaged_error_fixture_and_probe": "\"error_runtime_fixture\"" in e2e_text and "\"error_runtime_probe\"" in e2e_text,
        "result_bridge_consumer_exercises_bridged_paths": "extern fn bridged" in result_bridge_text and "try?" in result_bridge_text and "catch (NSError* error)" in result_bridge_text,
    }

    summary = {
        "issue": "error-runtime-closure-bridged-error-cross-module-compatibility",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_surface_count": len(contract["canonical_surfaces"]),
        "compatibility_rule_count": len(contract["compatibility_rules"]),
        "bridge_helper_symbol_count": len(contract["bridge_helper_symbols"]),
        "authoritative_case_id_count": len(contract["authoritative_case_ids"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Error Runtime Closure Bridge Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical surfaces: `{summary['canonical_surface_count']}`\n"
        f"- Compatibility rules: `{summary['compatibility_rule_count']}`\n"
        f"- Bridge helper symbols: `{summary['bridge_helper_symbol_count']}`\n"
        f"- Authoritative case ids: `{summary['authoritative_case_id_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
