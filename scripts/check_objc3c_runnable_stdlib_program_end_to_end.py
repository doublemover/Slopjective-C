#!/usr/bin/env python3
"""Validate the staged runnable stdlib program surface end to end."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_any as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
STDLIB_REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "runnable-end-to-end-summary.json"
SHOWCASE_REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "runnable-end-to-end-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-runnable-end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.program.runnable.end.to.end.summary.v1"






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    stdlib_result = run_capture(python_script_command(RUNNABLE_STDLIB_FOUNDATION_E2E_PY))
    if stdlib_result.returncode != 0:
        raise RuntimeError("runnable stdlib foundation validation failed")

    showcase_result = run_capture(python_script_command(RUNNABLE_SHOWCASE_E2E_PY))
    if showcase_result.returncode != 0:
        raise RuntimeError("runnable showcase validation failed")

    stdlib_summary = load_json(STDLIB_REPORT_PATH)
    showcase_summary = load_json(SHOWCASE_REPORT_PATH)

    packaged_program_surface = stdlib_summary.get("packaged_stdlib_program_surface", {})
    expect(isinstance(packaged_program_surface, dict), "runnable stdlib report missing packaged stdlib program surface")
    expect(
        isinstance(packaged_program_surface.get("publish_inputs"), list) and packaged_program_surface.get("publish_inputs"),
        "runnable stdlib report missing packaged publish inputs",
    )
    expect(
        isinstance(packaged_program_surface.get("examples"), list) and packaged_program_surface.get("examples"),
        "runnable stdlib report missing packaged capability demo examples",
    )
    expect(
        showcase_summary.get("contract_id") == "objc3c.showcase.runnable.end.to.end.summary.v1",
        "runnable showcase summary contract drifted",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_stdlib_program_end_to_end.py",
        "workflow_actions": [
            "validate-runnable-stdlib-foundation",
            "validate-runnable-showcase",
            "validate-runnable-stdlib-program",
        ],
        "child_report_paths": [
            repo_rel(STDLIB_REPORT_PATH),
            repo_rel(SHOWCASE_REPORT_PATH),
        ],
        "runnable_stdlib_foundation_summary": stdlib_summary,
        "runnable_showcase_summary": showcase_summary,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
