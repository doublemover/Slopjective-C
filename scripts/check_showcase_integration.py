#!/usr/bin/env python3
"""Validate showcase integration through the live checked-in compile and runtime paths."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SURFACE_REPORT = ROOT / "tmp" / "reports" / "showcase" / "summary.json"
RUNTIME_REPORT = ROOT / "tmp" / "reports" / "showcase" / "runtime-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.showcase.integration.summary.v1"
EXPECTED_EXAMPLE_IDS = ["auroraBoard", "signalMesh", "patchKit"]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
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
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def main() -> int:
    surface_result = run_capture([sys.executable, str(SHOWCASE_SURFACE_PY)])
    if surface_result.returncode != 0:
        raise RuntimeError("showcase surface validation failed")

    runtime_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SHOWCASE_RUNTIME_PS1)]
    )
    if runtime_result.returncode != 0:
        raise RuntimeError("showcase runtime validation failed")

    surface_summary = load_json(SURFACE_REPORT)
    runtime_summary = load_json(RUNTIME_REPORT)
    expect(
        surface_summary.get("contract_id") == "objc3c.showcase.surface.summary.v1",
        "showcase surface report published the wrong contract id",
    )
    expect(
        runtime_summary.get("contract_id") == "objc3c.showcase.runtime.summary.v1",
        "showcase runtime report published the wrong contract id",
    )

    selected_ids = surface_summary.get("selected_example_ids")
    expect(selected_ids == EXPECTED_EXAMPLE_IDS, "showcase surface report drifted from the full example set")

    runtime_examples = runtime_summary.get("examples")
    expect(isinstance(runtime_examples, list), "showcase runtime report did not publish examples")
    runtime_ids = [str(entry.get("example_id")) for entry in runtime_examples if isinstance(entry, dict)]
    expect(runtime_ids == EXPECTED_EXAMPLE_IDS, "showcase runtime report drifted from the full example set")

    actual_exit_codes = {
        str(entry.get("example_id")): entry.get("actual_exit_code")
        for entry in runtime_examples
        if isinstance(entry, dict)
    }
    expect(
        actual_exit_codes == {"auroraBoard": 33, "signalMesh": 13, "patchKit": 7},
        "showcase runtime report drifted from the expected runnable exits",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_showcase_integration.py",
        "example_ids": EXPECTED_EXAMPLE_IDS,
        "child_report_paths": [repo_rel(SURFACE_REPORT), repo_rel(RUNTIME_REPORT)],
        "showcase_surface_summary": surface_summary,
        "showcase_runtime_summary": runtime_summary,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
