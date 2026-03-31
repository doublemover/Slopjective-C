#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp/reports/concurrency-runtime-closure/closeout-gate"
JSON_OUT = OUT_DIR / "concurrency_runtime_closeout_gate.json"
MD_OUT = OUT_DIR / "concurrency_runtime_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.concurrency.runtime.closure.closeout.gate.v1"
COMMANDS = [
    [sys.executable, "scripts/build_concurrency_runtime_closure_boundary_inventory_summary.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_semantic_summary.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_scheduler_policy_summary.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_task_lifecycle_summary.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_actor_semantics_summary.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_artifact_summary.py"],
    [sys.executable, "scripts/check_concurrency_runtime_closure_task_lowering.py"],
    [sys.executable, "scripts/check_concurrency_runtime_closure_actor_lowering.py"],
    [sys.executable, "scripts/build_concurrency_runtime_closure_executable_proof_summary.py"],
    [sys.executable, "scripts/check_concurrency_runtime_closure_live_actor_runtime.py"],
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
        "runner_path": "scripts/check_m322_concurrency_runtime_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Concurrency Runtime Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Steps: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
