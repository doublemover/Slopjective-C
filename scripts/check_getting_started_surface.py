#!/usr/bin/env python3
"""Validate the bounded getting-started tutorial surface against the live docs and showcase examples."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
WALKTHROUGH_PATH = ROOT / "showcase" / "tutorial_walkthrough.json"
SHOWCASE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "showcase" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.tutorial.getting-started.surface.summary.v1"


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
    walkthrough = load_json(WALKTHROUGH_PATH)
    expect(
        walkthrough.get("contract_id") == "objc3c.showcase.tutorial.walkthrough.v1",
        "tutorial walkthrough contract drifted",
    )

    steps = walkthrough.get("steps")
    expect(isinstance(steps, list), "tutorial walkthrough did not publish steps")
    compile_steps = [
        step
        for step in steps
        if isinstance(step, dict) and str(step.get("public_entrypoint")) == "compile:objc3c"
    ]
    example_ids = [str(step.get("example_id")) for step in compile_steps]
    expect(
        example_ids == ["auroraBoard", "signalMesh", "patchKit"],
        "tutorial walkthrough compile example set drifted",
    )

    documentation_result = run_capture([sys.executable, str(DOCUMENTATION_SURFACE_PY)])
    if documentation_result.returncode != 0:
        raise RuntimeError("documentation surface validation failed")

    showcase_command = [sys.executable, str(SHOWCASE_SURFACE_PY)]
    for example_id in example_ids:
        showcase_command.extend(["--example", example_id])
    showcase_result = run_capture(showcase_command)
    if showcase_result.returncode != 0:
        raise RuntimeError("showcase surface validation failed for getting-started examples")

    showcase_summary = load_json(SHOWCASE_SUMMARY_PATH)
    expect(
        showcase_summary.get("contract_id") == "objc3c.showcase.surface.summary.v1",
        "showcase summary contract drifted",
    )
    expect(
        showcase_summary.get("selected_example_ids") == example_ids,
        "showcase summary drifted from the getting-started example set",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_getting_started_surface.py",
        "walkthrough_manifest": repo_rel(WALKTHROUGH_PATH),
        "example_ids": example_ids,
        "child_report_paths": [repo_rel(SHOWCASE_SUMMARY_PATH)],
        "showcase_surface_summary": showcase_summary,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
