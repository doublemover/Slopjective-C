#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/metaprogramming_interop_closure/executable_proof_abi_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_metaprogramming_interop_closure.md"
WORKFLOW_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
RUNTIME_HEADER_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
RUNTIME_CPP_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
OUT_DIR = ROOT / "tmp/reports/metaprogramming-interop-closure/executable-proof"
JSON_OUT = OUT_DIR / "metaprogramming_interop_closure_executable_proof_summary.json"
MD_OUT = OUT_DIR / "metaprogramming_interop_closure_executable_proof_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_PATH.read_text(encoding="utf-8")
    runtime_header_text = RUNTIME_HEADER_PATH.read_text(encoding="utf-8")
    runtime_cpp_text = RUNTIME_CPP_PATH.read_text(encoding="utf-8")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_metaprogramming_interop_closure_executable_proof_summary.py",
        "all_probe_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_probe_paths"]),
        "runbook_mentions_public_command_surface": all(command in runbook_text for command in contract["public_commands"]),
        "runbook_mentions_public_workflow_surface": all(workflow in runbook_text for workflow in contract["public_workflows"]),
        "workflow_preserves_public_command_surface": all(command in workflow_text for command in contract["public_workflows"]),
        "runtime_header_preserves_private_snapshot_symbols": all(symbol in runtime_header_text for symbol in contract["private_runtime_snapshot_symbols"]),
        "runtime_cpp_preserves_private_snapshot_symbols": all(symbol in runtime_cpp_text for symbol in contract["private_runtime_snapshot_symbols"]),
    }

    summary = {
        "issue": "metaprogramming-interop-closure-executable-proof-abi",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "canonical_report_count": len(contract["canonical_reports"]),
        "public_command_count": len(contract["public_commands"]),
        "public_workflow_count": len(contract["public_workflows"]),
        "private_runtime_snapshot_symbol_count": len(contract["private_runtime_snapshot_symbols"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Metaprogramming Interop Executable Proof Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical reports: `{summary['canonical_report_count']}`\n"
        f"- Public commands: `{summary['public_command_count']}`\n"
        f"- Public workflows: `{summary['public_workflow_count']}`\n"
        f"- Private runtime snapshot symbols: `{summary['private_runtime_snapshot_symbol_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
