#!/usr/bin/env python3
"""Run the integrated stdlib program validation path."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"
STDLIB_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
LLVM_CAPABILITIES_PROBE_PY = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-integration-summary.json"
CAPABILITY_REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-capability-explorer.json"
SHOWCASE_REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
GETTING_STARTED_REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-integration-summary.json"
STDLIB_FOUNDATION_REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.program.integration.summary.v1"


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
    documentation_result = run_capture([sys.executable, str(DOCUMENTATION_SURFACE_PY)])
    if documentation_result.returncode != 0:
        raise RuntimeError("documentation surface validation failed")

    showcase_result = run_capture([sys.executable, str(SHOWCASE_INTEGRATION_PY)])
    if showcase_result.returncode != 0:
        raise RuntimeError("showcase integration validation failed")

    getting_started_result = run_capture([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])
    if getting_started_result.returncode != 0:
        raise RuntimeError("getting-started integration validation failed")

    stdlib_foundation_result = run_capture([sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)])
    if stdlib_foundation_result.returncode != 0:
        raise RuntimeError("stdlib foundation integration validation failed")

    capability_result = run_capture(
        [
            sys.executable,
            str(LLVM_CAPABILITIES_PROBE_PY),
            "--summary-out",
            str(CAPABILITY_REPORT_PATH),
        ]
    )
    if capability_result.returncode != 0:
        raise RuntimeError("capability explorer validation failed")

    program_surface = load_json(PROGRAM_SURFACE_PATH)
    showcase_summary = load_json(SHOWCASE_REPORT_PATH)
    getting_started_summary = load_json(GETTING_STARTED_REPORT_PATH)
    stdlib_foundation_summary = load_json(STDLIB_FOUNDATION_REPORT_PATH)
    capability_summary = load_json(CAPABILITY_REPORT_PATH)

    expect(program_surface.get("contract_id") == "objc3c.stdlib.program_surface.v1", "program surface contract drifted")
    expect(
        showcase_summary.get("program_surface_contract") == repo_rel(PROGRAM_SURFACE_PATH),
        "showcase integration drifted from the stdlib program contract",
    )
    expect(
        getting_started_summary.get("program_surface_contract") == repo_rel(PROGRAM_SURFACE_PATH),
        "getting-started integration drifted from the stdlib program contract",
    )
    expect(
        capability_summary.get("capability_demo_compatibility", {}).get("ok") is True,
        "capability explorer did not keep capability_demo_compatibility ok=true",
    )
    expect(
        capability_summary.get("capability_demo_compatibility", {}).get("drift_checks", {}).get("actor_claims_are_qualified")
        is True,
        "capability explorer lost qualified actor-shaped capability claims",
    )
    expect(
        stdlib_foundation_summary.get("contract_id") == "objc3c.stdlib.foundation.integration.summary.v1",
        "stdlib foundation integration summary contract drifted",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_stdlib_program_integration.py",
        "program_surface_contract": repo_rel(PROGRAM_SURFACE_PATH),
        "workflow_actions": [
            "check-documentation-surface",
            "validate-showcase",
            "validate-getting-started",
            "validate-stdlib-foundation",
            "inspect-capability-explorer",
            "validate-stdlib-program",
        ],
        "child_report_paths": [
            repo_rel(SHOWCASE_REPORT_PATH),
            repo_rel(GETTING_STARTED_REPORT_PATH),
            repo_rel(STDLIB_FOUNDATION_REPORT_PATH),
            repo_rel(CAPABILITY_REPORT_PATH),
        ],
        "program_publish_inputs": program_surface.get("publish_inputs"),
        "capability_demo_examples": program_surface.get("capability_demo_examples"),
        "showcase_integration_summary": showcase_summary,
        "getting_started_integration_summary": getting_started_summary,
        "stdlib_foundation_integration_summary": stdlib_foundation_summary,
        "capability_explorer_summary": capability_summary,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
