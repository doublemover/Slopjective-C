#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "developer-tooling" / "closeout-gate"
JSON_OUT = OUT_DIR / "developer_tooling_closeout_gate.json"
MD_OUT = OUT_DIR / "developer_tooling_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.developer.tooling.closeout.gate.v1"
COMMANDS = [
    [sys.executable, "scripts/build_developer_tooling_boundary_inventory_summary.py"],
    [sys.executable, "scripts/build_developer_tooling_diagnostics_policy_summary.py"],
    [sys.executable, "scripts/build_developer_tooling_lsp_policy_summary.py"],
    [sys.executable, "scripts/build_developer_tooling_debug_semantics_summary.py"],
    [sys.executable, "scripts/build_developer_tooling_artifact_contract_summary.py"],
    [sys.executable, "scripts/check_developer_tooling_language_server_navigation.py"],
    [sys.executable, "scripts/check_developer_tooling_formatter_debug_surface.py"],
    [sys.executable, "scripts/check_developer_tooling_workspace_integration.py"],
    [sys.executable, "scripts/check_objc3c_developer_tooling_integration.py"],
    [sys.executable, "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py"],
    [sys.executable, "scripts/render_objc3c_public_command_surface.py"],
    [sys.executable, "scripts/build_objc3c_public_command_contract.py"],
    [sys.executable, "scripts/check_documentation_surface.py"],
    [sys.executable, "scripts/check_repo_superclean_surface.py"],
]


def run_command(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    return {
        "command": command,
        "exit_code": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "ok": result.returncode == 0,
    }


def main() -> int:
    steps = [run_command(command) for command in COMMANDS]
    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if all(step["ok"] for step in steps) else "FAIL",
        "runner_path": "scripts/check_m325_developer_tooling_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Developer Tooling Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Steps: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
