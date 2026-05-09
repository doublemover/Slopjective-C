#!/usr/bin/env python3
"""Run the bounded getting-started validation surface plus live showcase runtime smoke."""

from __future__ import annotations

import json
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
GETTING_STARTED_SURFACE_PY = ROOT / "scripts" / "check_getting_started_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SURFACE_REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-surface-summary.json"
RUNTIME_REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "runtime-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.tutorial.getting-started.integration.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    surface_result = run_capture(python_script_command(GETTING_STARTED_SURFACE_PY))
    if surface_result.returncode != 0:
        raise RuntimeError("getting-started surface validation failed")

    surface_summary = load_json(SURFACE_REPORT_PATH)
    expect(
        surface_summary.get("contract_id") == "objc3c.tutorial.getting-started.surface.summary.v1",
        "getting-started surface summary contract drifted",
    )
    example_ids = surface_summary.get("example_ids")
    expect(example_ids == ["auroraBoard", "signalMesh", "patchKit"], "getting-started example set drifted")

    example_arg = ",".join(str(example_id) for example_id in example_ids)
    runtime_command = [
        PWSH,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-Command",
        f"& '{SHOWCASE_RUNTIME_PS1}' -Example {example_arg}",
    ]
    runtime_result = run_capture(runtime_command)
    if runtime_result.returncode != 0:
        raise RuntimeError("getting-started showcase runtime smoke failed")

    runtime_summary = load_json(RUNTIME_REPORT_PATH)
    expect(
        runtime_summary.get("contract_id") == "objc3c.showcase.runtime.summary.v1",
        "showcase runtime summary contract drifted",
    )
    runtime_examples = runtime_summary.get("examples")
    expect(isinstance(runtime_examples, list), "showcase runtime summary did not publish examples")
    runtime_ids = [str(entry.get("example_id")) for entry in runtime_examples if isinstance(entry, dict)]
    expect(runtime_ids == example_ids, "showcase runtime example set drifted from the getting-started surface")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_getting_started_integration.py",
        "program_surface_contract": surface_summary.get("program_surface_contract"),
        "program_publish_inputs": surface_summary.get("program_publish_inputs"),
        "capability_demo_examples": surface_summary.get("capability_demo_examples"),
        "example_ids": example_ids,
        "child_report_paths": [repo_rel(SURFACE_REPORT_PATH), repo_rel(RUNTIME_REPORT_PATH)],
        "getting_started_surface_summary": surface_summary,
        "showcase_runtime_summary": runtime_summary,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
