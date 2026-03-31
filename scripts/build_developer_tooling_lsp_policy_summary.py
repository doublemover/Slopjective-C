#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/language_server_capability_fallback_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_developer_tooling.md"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/lsp-policy"
JSON_OUT = OUT_DIR / "language_server_capability_fallback_policy_summary.json"
MD_OUT = OUT_DIR / "language_server_capability_fallback_policy_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_developer_tooling_lsp_policy_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "runbook_has_language_server_heading": "## Language-Server Capability And Fallback Policy" in runbook_text,
        "runbook_mentions_capability_map": "one canonical capability map" in runbook_text,
        "runbook_mentions_compile_owned_diagnostics": "compile-owned diagnostics" in runbook_text,
        "runbook_mentions_compile_owned_declaration_coordinates": "compile-owned declaration coordinates" in runbook_text,
        "runbook_mentions_emitted_artifact_presence": "emitted artifact presence" in runbook_text,
        "runbook_mentions_fallback_capabilities": all(capability in runbook_text for capability in ["references", "rename", "semantic tokens", "code actions"]),
        "runbook_mentions_statement_level_stepping_fallback": "statement-level debugger stepping" in runbook_text,
    }

    summary = {
        "issue": "developer-tooling-language-server-capability-fallback-policy",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "capability_class_count": len(contract["capability_classes"]),
        "fallback_only_capability_class_count": len(contract["fallback_only_capability_classes"]),
        "evidence_root_count": len(contract["evidence_roots"]),
        "claim_narrowing_constraint_count": len(contract["claim_narrowing_constraints"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Developer Tooling Language-Server Capability Policy Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Capability classes: `{summary['capability_class_count']}`\n"
        f"- Fallback-only capability classes: `{summary['fallback_only_capability_class_count']}`\n"
        f"- Evidence roots: `{summary['evidence_root_count']}`\n"
        f"- Claim-narrowing constraints: `{summary['claim_narrowing_constraint_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
