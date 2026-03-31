#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/executable_proof_abi_contract.json"
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/executable-proof-abi"
JSON_OUT = OUT_DIR / "executable_proof_abi_summary.json"
MD_OUT = OUT_DIR / "executable_proof_abi_summary.md"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_concurrency_runtime_closure.md"
PACKAGE_JSON_PATH = ROOT / "package.json"
WORKFLOW_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
COMMAND_SURFACE_PATH = ROOT / "docs/runbooks/objc3c_public_command_surface.md"
RUNTIME_PATH = ROOT / "native/objc3c/src/runtime/objc3_runtime.cpp"
CONFORMANCE_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-conformance/summary.json"
E2E_REPORT = ROOT / "tmp/reports/runtime/runnable-concurrency-e2e/summary.json"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    package_text = PACKAGE_JSON_PATH.read_text(encoding="utf-8")
    workflow_text = WORKFLOW_PATH.read_text(encoding="utf-8")
    command_surface_text = COMMAND_SURFACE_PATH.read_text(encoding="utf-8")
    runtime_text = RUNTIME_PATH.read_text(encoding="utf-8")
    conformance_report = read_json(CONFORMANCE_REPORT)
    e2e_report = read_json(E2E_REPORT)

    fixture_paths = [ROOT / path for path in contract["authoritative_fixture_paths"]]
    probe_paths = [ROOT / path for path in contract["authoritative_probe_paths"]]
    code_paths = [ROOT / path for path in contract["authoritative_code_paths"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_concurrency_runtime_closure_executable_proof_summary.py",
        "all_authoritative_code_paths_exist": all(path.is_file() for path in code_paths),
        "all_authoritative_fixture_paths_exist": all(path.is_file() for path in fixture_paths),
        "all_authoritative_probe_paths_exist": all(path.is_file() for path in probe_paths),
        "runbook_mentions_public_command_boundary": "test:objc3c:concurrency-conformance" in runbook_text and "test:objc3c:runnable-concurrency" in runbook_text,
        "package_json_preserves_public_commands": all(command in package_text for command in contract["public_commands"]),
        "workflow_preserves_public_actions": all(action in workflow_text for action in contract["public_workflows"]),
        "command_surface_documents_public_commands": all(command in command_surface_text for command in contract["public_commands"]) and all(action in command_surface_text for action in contract["public_workflows"]),
        "runtime_exports_private_concurrency_abi_boundary": all(symbol in runtime_text for symbol in contract["private_runtime_abi_boundary"]),
        "conformance_report_passes": conformance_report.get("status") == "PASS",
        "e2e_report_passes": e2e_report.get("status") == "PASS",
    }

    summary = {
        "issue": "concurrency-runtime-closure-executable-proof-abi-contract",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "public_command_count": len(contract["public_commands"]),
        "public_workflow_count": len(contract["public_workflows"]),
        "private_runtime_abi_symbol_count": len(contract["private_runtime_abi_boundary"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closure Executable Proof ABI Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Public commands: `{summary['public_command_count']}`\n"
        f"- Public workflows: `{summary['public_workflow_count']}`\n"
        f"- Private ABI symbols: `{summary['private_runtime_abi_symbol_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
