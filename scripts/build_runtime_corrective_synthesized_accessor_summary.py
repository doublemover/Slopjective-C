#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/runtime_corrective/synthesized_accessor_semantic_model.json"
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/synthesized-accessor-semantic"
JSON_OUT = OUT_DIR / "synthesized_accessor_semantic_summary.json"
MD_OUT = OUT_DIR / "synthesized_accessor_semantic_summary.md"
LOWERING_CONTRACT_PATH = ROOT / "native/objc3c/src/lower/objc3_lowering_contract.h"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
ACCEPTANCE_SCRIPT_PATH = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_literal(text: str, needle: str) -> int:
    return text.count(needle)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    lowering_text = LOWERING_CONTRACT_PATH.read_text(encoding="utf-8")
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    acceptance_text = ACCEPTANCE_SCRIPT_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]

    checks = {
        "runbook_link_matches": contract["runbook"] == "docs/runbooks/objc3c_runtime_corrective.md",
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_runtime_corrective_synthesized_accessor_summary.py",
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "lowering_contract_path_matches": contract["lowering_contract_source_path"] == "native/objc3c/src/lower/objc3_lowering_contract.h",
        "helper_selection_model_present_in_lowering_contract": contract["accessor_storage_lowering_helper_selection_model"] in lowering_text,
        "runtime_storage_execution_models_present_in_lowering_contract": all(
            model in lowering_text for model in contract["runtime_storage_execution_models"]
        ),
        "helper_symbols_present_in_runtime": all(
            symbol in runtime_text for symbol in contract["helper_symbol_expectations"].values()
        ),
        "acceptance_script_uses_same_helper_selection_model": contract["accessor_storage_lowering_helper_selection_model"] in acceptance_text,
        "acceptance_script_publishes_baseline_counts": all(
            f'("{key}", {value})' in acceptance_text
            or f'get("{key}") == {str(value)}' in acceptance_text
            for key, value in contract["baseline_lowering_counts"].items()
        ),
        "acceptance_script_publishes_arc_counts": all(
            f'get("{key}") == {str(value)}' in acceptance_text
            for key, value in contract["arc_interaction_counts"].items()
        ),
        "runbook_mentions_synthesized_accessor_contract": "synthesized_accessor_semantic_model.json" in runbook_text,
        "non_goals_keep_closure_narrow": "no-claim-that-full-atomic-or-reflection-closure-has-landed-in-b002" in contract["explicit_non_goals"],
    }

    measured_inventory = {
        "authoritative_fixture_count": len(contract["authoritative_fixture_paths"]),
        "authoritative_probe_count": len(contract["authoritative_probe_paths"]),
        "runtime_storage_execution_model_count": len(contract["runtime_storage_execution_models"]),
        "helper_symbol_count": len(contract["helper_symbol_expectations"]),
        "baseline_lowering_counter_count": len(contract["baseline_lowering_counts"]),
        "arc_interaction_counter_count": len(contract["arc_interaction_counts"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "current_property_occurrences_in_runtime": count_literal(runtime_text, "current_property"),
        "weak_current_property_occurrences_in_runtime": count_literal(runtime_text, "weak_current_property"),
        "helper_selection_model_occurrences_in_acceptance_script": count_literal(
            acceptance_text, contract["accessor_storage_lowering_helper_selection_model"]
        ),
    }

    summary = {
        "issue": "runtime-corrective-synthesized-accessor-semantic",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "measured_inventory": measured_inventory,
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Runtime Corrective Synthesized Accessor Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Authoritative fixtures: `{measured_inventory['authoritative_fixture_count']}`\n"
        f"- Authoritative probes: `{measured_inventory['authoritative_probe_count']}`\n"
        f"- Helper symbols: `{measured_inventory['helper_symbol_count']}`\n"
        f"- Baseline lowering counters: `{measured_inventory['baseline_lowering_counter_count']}`\n"
        f"- ARC interaction counters: `{measured_inventory['arc_interaction_counter_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
