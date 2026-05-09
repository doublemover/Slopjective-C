#!/usr/bin/env python3
"""Run the integrated stdlib program validation path."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_any as load_json, write_json_file
from objc3c_tooling.subprocesses import python_script_command, run_capture


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






def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    documentation_result = run_capture(python_script_command(DOCUMENTATION_SURFACE_PY))
    if documentation_result.returncode != 0:
        raise RuntimeError("documentation surface validation failed")

    showcase_result = run_capture(python_script_command(SHOWCASE_INTEGRATION_PY))
    if showcase_result.returncode != 0:
        raise RuntimeError("showcase integration validation failed")

    getting_started_result = run_capture(python_script_command(GETTING_STARTED_INTEGRATION_PY))
    if getting_started_result.returncode != 0:
        raise RuntimeError("getting-started integration validation failed")

    stdlib_foundation_result = run_capture(python_script_command(STDLIB_FOUNDATION_INTEGRATION_PY))
    if stdlib_foundation_result.returncode != 0:
        raise RuntimeError("stdlib foundation integration validation failed")

    capability_result = run_capture(
        python_script_command(
            LLVM_CAPABILITIES_PROBE_PY,
            "--summary-out",
            str(CAPABILITY_REPORT_PATH),
        )
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
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
