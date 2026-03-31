#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/block_arc_closure/executable_proof_abi_contract.json"
PACKAGE_PATH = ROOT / "package.json"
RUNNER_PATH = ROOT / "scripts/objc3c_public_workflow_runner.py"
OUT_DIR = ROOT / "tmp/reports/m320/M320-D001"
JSON_OUT = OUT_DIR / "block_arc_closure_executable_proof_summary.json"
MD_OUT = OUT_DIR / "block_arc_closure_executable_proof_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    package_text = PACKAGE_PATH.read_text(encoding="utf-8")
    runner_text = RUNNER_PATH.read_text(encoding="utf-8")
    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_block_arc_closure_executable_proof_summary.py",
        "all_authoritative_runner_paths_exist": all((ROOT / path).is_file() for path in contract["authoritative_runner_paths"]),
        "all_public_command_surfaces_exist": all(command in package_text for command in contract["public_command_surfaces"]),
        "all_public_workflow_actions_exist": all(action in runner_text for action in contract["public_workflow_actions"]),
    }

    summary = {
        "issue": "M320-D001",
        "contract_id": contract["contract_id"],
        "surface_kind": contract["surface_kind"],
        "public_command_surface_count": len(contract["public_command_surfaces"]),
        "public_workflow_action_count": len(contract["public_workflow_actions"]),
        "public_runtime_abi_symbol_count": len(contract["public_runtime_abi_boundary"]),
        "private_runtime_abi_symbol_count": len(contract["private_runtime_abi_boundary"]),
        "checks": checks,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M320-D001 Block ARC Closure Executable Proof Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Public commands: `{summary['public_command_surface_count']}`\n"
        f"- Public workflow actions: `{summary['public_workflow_action_count']}`\n"
        f"- Public ABI symbols: `{summary['public_runtime_abi_symbol_count']}`\n"
        f"- Private ABI symbols: `{summary['private_runtime_abi_symbol_count']}`\n"
        f"- Status: `{summary['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
