#!/usr/bin/env python3
"""Run the integrated stdlib foundation validation path."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.foundation.integration.summary.v1"
SURFACE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "surface-summary.json"
SMOKE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "workspace-smoke-summary.json"


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


def main() -> int:
    surface_result = run_capture([sys.executable, str(RUNNER), "check-stdlib-surface"])
    if surface_result.returncode != 0:
        raise RuntimeError("stdlib surface validation failed")

    smoke_result = run_capture([sys.executable, str(ROOT / "scripts" / "run_objc3c_stdlib_workspace_smoke.py")])
    if smoke_result.returncode != 0:
        raise RuntimeError("stdlib workspace smoke validation failed")

    surface_summary = load_json(SURFACE_SUMMARY_PATH)
    smoke_summary = load_json(SMOKE_SUMMARY_PATH)
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "generated_at_utc": datetime.now(timezone.utc).isoformat(),
                "status": "PASS",
                "child_report_paths": [repo_rel(SURFACE_SUMMARY_PATH), repo_rel(SMOKE_SUMMARY_PATH)],
                "surface_summary": surface_summary,
                "workspace_smoke_summary": smoke_summary,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
