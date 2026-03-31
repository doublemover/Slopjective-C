#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp/reports/runtime-corrective/closeout-gate"
SUMMARY_PATH = OUT_DIR / "runtime_corrective_closeout_gate.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"

COMMANDS = [
    {
        "name": "dispatch-lowering-proof",
        "command": ["python", "scripts/check_runtime_corrective_dispatch_lowering.py"],
        "summary_path": ROOT / "tmp/reports/runtime-corrective/dispatch-lowering-proof/dispatch_lowering_implementation_summary.json",
    },
    {
        "name": "synthesized-accessor-lowering-proof",
        "command": ["python", "scripts/check_runtime_corrective_synthesized_accessor_lowering.py"],
        "summary_path": ROOT / "tmp/reports/runtime-corrective/synthesized-accessor-lowering-proof/synthesized_accessor_lowering_implementation_summary.json",
    },
    {
        "name": "executable-proof-contract",
        "command": ["python", "scripts/build_runtime_corrective_executable_proof_summary.py"],
        "summary_path": ROOT / "tmp/reports/runtime-corrective/executable-proof-abi/executable_proof_abi_contract_summary.json",
    },
    {
        "name": "live-dispatch-runtime-proof",
        "command": ["python", "scripts/check_runtime_corrective_live_dispatch_runtime.py"],
        "summary_path": ROOT / "tmp/reports/runtime-corrective/live-dispatch-runtime/live_dispatch_runtime_summary.json",
    },
    {
        "name": "synthesized-accessor-runtime-proof",
        "command": ["python", "scripts/check_runtime_corrective_synthesized_accessor_runtime.py"],
        "summary_path": ROOT / "tmp/reports/runtime-corrective/live-synthesized-accessor-runtime/synthesized_accessor_runtime_summary.json",
    },
    {
        "name": "documentation-surface",
        "command": ["python", "scripts/check_documentation_surface.py"],
        "summary_path": None,
    },
    {
        "name": "repo-superclean-surface",
        "command": ["python", "scripts/check_repo_superclean_surface.py"],
        "summary_path": None,
    },
]


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)


def main() -> int:
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    command_results: list[dict[str, Any]] = []
    ok = True
    for entry in COMMANDS:
        result = run_command(entry["command"])
        entry_ok = result.returncode == 0
        payload: dict[str, Any] = {
            "name": entry["name"],
            "command": " ".join(entry["command"]),
            "exit_code": result.returncode,
            "ok": entry_ok,
        }
        if entry["summary_path"] is not None and entry["summary_path"].is_file():
            summary_json = read_json(entry["summary_path"])
            payload["summary_path"] = str(entry["summary_path"].relative_to(ROOT)).replace("\\", "/")
            payload["summary_ok"] = bool(summary_json.get("ok", False))
            entry_ok = entry_ok and payload["summary_ok"]
        else:
            payload["summary_path"] = None
        if not entry_ok:
            payload["stdout"] = result.stdout
            payload["stderr"] = result.stderr
        payload["ok"] = entry_ok
        command_results.append(payload)
        ok = ok and entry_ok

    summary = {
        "issue": "runtime-corrective-closeout-gate",
        "runbook_mentions_closeout_gate": "check_runtime_corrective_closeout_gate.py" in runbook_text,
        "command_count": len(COMMANDS),
        "commands": command_results,
    }
    summary["ok"] = ok and summary["runbook_mentions_closeout_gate"]

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
