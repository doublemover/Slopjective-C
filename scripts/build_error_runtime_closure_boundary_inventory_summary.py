#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json"
OUT_DIR = ROOT / "tmp/reports/m321/M321-A001"
JSON_OUT = OUT_DIR / "error_runtime_closure_boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "error_runtime_closure_boundary_inventory_summary.md"
DOC_PATH = ROOT / "docs/objc3c-native.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_error_runtime_closure.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
PACKAGE_PATH = ROOT / "package.json"
WORKFLOW_RUNNER_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    doc_text = DOC_PATH.read_text(encoding="utf-8")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    package_text = PACKAGE_PATH.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_RUNNER_PATH.read_text(encoding="utf-8")

    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_error_runtime_closure_boundary_inventory_summary.py",
        "runbook_exists": RUNBOOK_PATH.is_file(),
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_runtime_symbols_exported": all(symbol in runtime_text for symbol in contract["authoritative_runtime_symbols"]),
        "docs_publish_error_source_surface": "## Error Execution And Cleanup Source Surface" in doc_text,
        "docs_publish_error_lowering_surface": "## Error Lowering Unwind And Bridge Helper Surface" in doc_text,
        "docs_publish_error_runtime_abi_surface": "## Error Runtime ABI And Cleanup Surface" in doc_text,
        "docs_publish_error_runtime_impl_surface": "## Error Propagation Catch And Cleanup Runtime Implementation Surface" in doc_text,
        "runbook_mentions_private_runtime_helper_constraint": "error behavior stays on the private runtime-owned helper and snapshot surfaces" in runbook_text,
        "public_command_surfaces_exist": all(command in package_text for command in contract["public_command_surfaces"]),
        "public_workflow_actions_exist": all(action in workflow_text for action in contract["public_workflow_actions"]),
    }

    symbol_occurrence_counts = {
        symbol: runtime_text.count(symbol) for symbol in contract["authoritative_runtime_symbols"]
    }
    doc_occurrence_counts = {
        heading: doc_text.count(heading) for heading in (
            "## Error Execution And Cleanup Source Surface",
            "## Error Lowering Unwind And Bridge Helper Surface",
            "## Error Runtime ABI And Cleanup Surface",
            "## Error Propagation Catch And Cleanup Runtime Implementation Surface",
        )
    }

    summary = {
        "issue": "M321-A001",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "focus_track_count": len(contract["focus_tracks"]),
        "authoritative_surface_key_count": len(contract["authoritative_surface_keys"]),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "authoritative_probe_path_count": len(contract["authoritative_probe_paths"]),
        "authoritative_fixture_path_count": len(contract["authoritative_fixture_paths"]),
        "current_gap_count": len(contract["current_gaps"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_milestone_count": len(contract["successor_milestones"]),
        "runtime_symbol_occurrence_counts": symbol_occurrence_counts,
        "documentation_occurrence_counts": doc_occurrence_counts,
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values())
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M321-A001 Error Runtime Closure Boundary Inventory Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Focus tracks: `{summary['focus_track_count']}`\n"
        f"- Surface keys: `{summary['authoritative_surface_key_count']}`\n"
        f"- Code paths: `{summary['authoritative_code_path_count']}`\n"
        f"- Runtime symbols: `{summary['authoritative_runtime_symbol_count']}`\n"
        f"- Probe paths: `{summary['authoritative_probe_path_count']}`\n"
        f"- Fixture paths: `{summary['authoritative_fixture_path_count']}`\n"
        f"- Current gaps: `{summary['current_gap_count']}`\n"
        f"- Explicit non-goals: `{summary['explicit_non_goal_count']}`\n"
        f"- Successor milestones: `{summary['successor_milestone_count']}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
