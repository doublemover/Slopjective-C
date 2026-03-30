#!/usr/bin/env python3
"""Validate the staged runnable stdlib program surface end to end."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
STDLIB_REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "runnable-end-to-end-summary.json"
SHOWCASE_REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "runnable-end-to-end-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-runnable-end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.program.runnable.end.to.end.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: list[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    stdlib_result = run_capture([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])
    if stdlib_result.returncode != 0:
        raise RuntimeError("runnable stdlib foundation validation failed")

    showcase_result = run_capture([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])
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
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
