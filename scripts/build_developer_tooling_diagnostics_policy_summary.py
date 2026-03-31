#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/diagnostics_formatting_symbol_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_developer_tooling.md"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/diagnostics-policy"
JSON_OUT = OUT_DIR / "diagnostics_formatting_symbol_policy_summary.json"
MD_OUT = OUT_DIR / "diagnostics_formatting_symbol_policy_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]
    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_developer_tooling_diagnostics_policy_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "runbook_mentions_diagnostics_source_of_truth": "diagnostics source of truth:" in runbook_text,
        "runbook_mentions_emitted_diagnostics_json": "emitted diagnostics JSON with real line, column, severity, code, and" in runbook_text,
        "runbook_mentions_manifest_declaration_records": "symbol resolution source of truth:" in runbook_text,
        "runbook_mentions_manifest_coordinates": "compile-owned declaration" in runbook_text and "shadow symbol index" in runbook_text,
        "runbook_mentions_formatter_fail_closed": "formatter claims must fail closed when the source is outside the supported" in runbook_text,
        "runbook_mentions_no_shadow_parser": "no editor-only shadow parser may become the source of truth for diagnostics or symbol resolution" in contract["claim_narrowing_constraints"],
    }

    summary = {
        "issue": "developer-tooling-diagnostics-formatting-symbol-policy",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "diagnostics_source_count": len(contract["diagnostics_source_of_truth"]),
        "symbol_resolution_source_count": len(contract["symbol_resolution_source_of_truth"]),
        "formatting_policy_count": len(contract["formatting_policy"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Developer Tooling Diagnostics, Formatting, And Symbol Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Diagnostics sources: `{summary['diagnostics_source_count']}`\n"
        f"- Symbol-resolution sources: `{summary['symbol_resolution_source_count']}`\n"
        f"- Formatting policy entries: `{summary['formatting_policy_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
