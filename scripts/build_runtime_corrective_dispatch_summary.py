#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/realized_dispatch_semantic_model.json"
SUMMARY_OUT_DIR = ROOT / "tmp/reports/runtime-corrective/realized-dispatch-semantic"
JSON_OUT = SUMMARY_OUT_DIR / "realized_dispatch_semantic_summary.json"
MD_OUT = SUMMARY_OUT_DIR / "realized_dispatch_semantic_summary.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"
ACCEPTANCE_SCRIPT_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(text: str, needle: str) -> int:
    return text.count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    acceptance_text = ACCEPTANCE_SCRIPT_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_dispatch_summary.py",
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "dispatch_entrypoint_present": "int objc3_runtime_dispatch_i32(" in runtime_text,
        "slow_path_resolver_present": "SlowPathResolution ResolveMethodSlowPathUnlocked(" in runtime_text,
        "method_cache_snapshot_present": "int objc3_runtime_copy_method_cache_state_for_testing(" in runtime_text,
        "method_cache_entry_snapshot_present": "int objc3_runtime_copy_method_cache_entry_for_testing(" in runtime_text,
        "realized_class_snapshot_present": "int objc3_runtime_copy_realized_class_entry_for_testing(" in runtime_text,
        "protocol_query_snapshot_present": "int objc3_runtime_copy_protocol_conformance_query_for_testing(" in runtime_text,
        "runtime_contains_all_dispatch_path_labels": all(label in runtime_text for label in contract["dispatch_path_labels"]),
        "docs_still_publish_dispatch_fallback_gap": "unresolved dispatch still has one deterministic fallback path after slow-path miss" in doc_text,
        "acceptance_script_uses_same_lookup_resolution_model": contract["lookup_resolution_order_model"] in acceptance_text,
        "acceptance_script_uses_same_unresolved_behavior_model": contract["unresolved_selector_behavior_model"] in acceptance_text,
        "runbook_mentions_realized_dispatch_contract": "realized_dispatch_semantic_model.json" in runbook_text,
        "non_goals_keep_full_closure_out_of_scope": "no-claim-that-category-protocol-reflection-closure-is-complete" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "runtime_acceptance_case_count": len(contract["runtime_acceptance_case_ids"]),
        "authoritative_fixture_count": len(contract["authoritative_fixture_paths"]),
        "authoritative_probe_count": len(contract["authoritative_probe_paths"]),
        "private_lookup_query_symbol_count": len(contract["private_lookup_query_boundary"]),
        "dispatch_counter_expectation_count": len(contract["dispatch_counter_expectations"]),
        "dispatch_path_label_count": len(contract["dispatch_path_labels"]),
        "claim_boundary_clause_count": len(contract["claim_boundary"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "runtime_dispatch_occurrences": count_literal(runtime_text, "objc3_runtime_dispatch_i32"),
        "method_cache_occurrences": count_literal(runtime_text, "method_cache"),
        "fallback_formula_occurrences": count_literal(runtime_text, "fallback-formula"),
        "nil_short_circuit_occurrences": count_literal(runtime_text, "nil-short-circuit"),
    }

    summary = {
        "issue": "runtime-corrective-realized-dispatch-semantic",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "runtime_acceptance_case_ids": contract["runtime_acceptance_case_ids"],
        "measured_inventory": measured_inventory,
        "checks": checks,
        "ok": all(checks.values()),
    }

    SUMMARY_OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Runtime Corrective Dispatch Semantic Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Runtime-acceptance cases: `{', '.join(summary['runtime_acceptance_case_ids'])}`\n"
        f"- Authoritative fixtures: `{measured_inventory['authoritative_fixture_count']}`\n"
        f"- Authoritative probes: `{measured_inventory['authoritative_probe_count']}`\n"
        f"- Dispatch path labels: `{measured_inventory['dispatch_path_label_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
