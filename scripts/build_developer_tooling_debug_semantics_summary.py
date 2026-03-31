#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/debugger_source_map_stepping_semantics.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_developer_tooling.md"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/debug-semantics"
JSON_OUT = OUT_DIR / "debugger_source_map_stepping_semantics_summary.json"
MD_OUT = OUT_DIR / "debugger_source_map_stepping_semantics_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_developer_tooling_debug_semantics_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_has_debug_heading": "## Debugger, Source-Map, And Stepping Semantics" in runbook_text,
        "runbook_mentions_breakpoint_navigation_anchors": "breakpoint and navigation anchors come from compile-owned declaration" in runbook_text,
        "runbook_mentions_object_symbol_visibility": "object-backed symbol visibility comes from the emitted object artifact" in runbook_text,
        "runbook_mentions_statement_level_fail_closed": "statement-level stepping and full source-map claims remain fail-closed" in runbook_text,
        "runbook_mentions_declaration_breakpoint_model": "declaration-breakpoint and artifact-inspection driven" in runbook_text,
    }

    summary = {
        "issue": "developer-tooling-debugger-source-map-stepping-semantics",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "debug_anchor_source_count": len(contract["debug_anchor_sources"]),
        "fail_closed_claim_count": len(contract["fail_closed_claims"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Developer Tooling Debugger, Source-Map, And Stepping Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Debug anchor sources: `{summary['debug_anchor_source_count']}`\n"
        f"- Fail-closed claims: `{summary['fail_closed_claim_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
