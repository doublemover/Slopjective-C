#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/boundary_inventory.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
METAPROGRAMMING_REPORT = ROOT / "tmp/reports/runtime/runnable-metaprogramming-conformance/summary.json"
INTEROP_REPORT = ROOT / "tmp/reports/runtime/runnable-interop-conformance/summary.json"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/boundary-inventory"
JSON_OUT = OUT_DIR / "boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "boundary_inventory_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    metaprogramming_report = read_json(METAPROGRAMMING_REPORT)
    interop_report = read_json(INTEROP_REPORT)
    runtime_text = (ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp").read_text(encoding="utf-8")
    package_text = (ROOT / "package.json").read_text(encoding="utf-8")
    workflow_text = (ROOT / "scripts/objc3c_public_workflow_runner.py").read_text(encoding="utf-8")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_boundary_inventory_summary.py",
        "runbook_exists": RUNBOOK_PATH.is_file(),
        "all_authoritative_code_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_code_paths"]),
        "all_authoritative_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "all_authoritative_fixture_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_fixture_paths"]),
        "all_authoritative_runtime_symbols_exported": all(symbol in runtime_text for symbol in contract["authoritative_runtime_symbols"]),
        "runbook_mentions_track_split": "metaprogramming/property-behavior closure and interop closure are separate internal tracks" in runbook_text,
        "runbook_mentions_property_behavior_runtime_materialization": "property behaviors must be treated as runtime-observable behavior" in runbook_text,
        "runbook_mentions_interop_abi_constraint": "interop claims must pass through ABI, ownership, error, and async compatibility on the real packaged toolchain" in runbook_text,
        "metaprogramming_report_passes": metaprogramming_report.get("status") == "PASS",
        "interop_report_passes": interop_report.get("status") == "PASS",
        "metaprogramming_report_publishes_required_surface_keys": all(
            key in metaprogramming_report.get("required_surface_keys", [])
            or key in metaprogramming_report
            for key in contract["authoritative_metaprogramming_surface_keys"]
        ),
        "interop_report_publishes_required_surface_keys": all(
            key in interop_report.get("required_surface_keys", [])
            or key in interop_report
            for key in contract["authoritative_interop_surface_keys"]
        ),
        "package_json_preserves_public_commands": all(command in package_text for command in contract["public_commands"]),
        "workflow_runner_preserves_public_actions": all(action.replace("-", "_") in workflow_text for action in contract["public_workflows"]),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-boundary-inventory",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "focus_track_count": len(contract["focus_tracks"]),
        "metaprogramming_surface_key_count": len(contract["authoritative_metaprogramming_surface_keys"]),
        "interop_surface_key_count": len(contract["authoritative_interop_surface_keys"]),
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "authoritative_probe_path_count": len(contract["authoritative_probe_paths"]),
        "authoritative_fixture_path_count": len(contract["authoritative_fixture_paths"]),
        "authoritative_runtime_symbol_count": len(contract["authoritative_runtime_symbols"]),
        "metaprogramming_required_case_count": len(metaprogramming_report.get("required_case_ids", [])),
        "interop_required_case_count": len(interop_report.get("required_case_ids", [])),
        "current_gap_count": len(contract["current_gaps"]),
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_milestone_count": len(contract["successor_milestones"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Metaprogramming/Interop Closure Boundary Inventory Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Focus tracks: `{summary['focus_track_count']}`\n"
        f"- Metaprogramming surface keys: `{summary['metaprogramming_surface_key_count']}`\n"
        f"- Interop surface keys: `{summary['interop_surface_key_count']}`\n"
        f"- Metaprogramming required cases: `{summary['metaprogramming_required_case_count']}`\n"
        f"- Interop required cases: `{summary['interop_required_case_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
