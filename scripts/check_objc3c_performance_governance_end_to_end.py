#!/usr/bin/env python3
"""Validate the performance-governance workflow end to end."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_payload,
    public_workflow_command,
    public_workflow_has_actions,
)
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "performance-governance" / "integration-summary.json"
END_TO_END_REPORT = ROOT / "tmp" / "reports" / "performance-governance" / "end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.end_to_end.summary.v1"



def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def ensure_integration_report() -> dict[str, Any]:
    if INTEGRATION_REPORT.is_file():
        report = load_json(INTEGRATION_REPORT)
        if report.get("status") == "PASS":
            return report
    completed = run_capture(public_workflow_command("validate-performance-governance-integration"))
    expect(completed.returncode == 0, "validate-performance-governance-integration failed during end-to-end validation")
    return load_json(INTEGRATION_REPORT)


def describe_action(action: str) -> dict[str, Any]:
    payload = public_workflow_action_payload(action)
    expect(isinstance(payload, dict), f"--describe {action} did not return a JSON object")
    return payload


def main() -> int:
    integration = ensure_integration_report()
    expect(integration.get("status") == "PASS", "performance-governance integration summary did not pass")

    validate_desc = describe_action("validate-performance-governance")
    integration_desc = describe_action("validate-performance-governance-integration")
    end_to_end_desc = describe_action("validate-performance-governance-end-to-end")
    ci_desc = describe_action("test-ci")
    nightly_desc = describe_action("test-nightly")

    expect(validate_desc.get("action") == "validate-performance-governance", "validate-performance-governance description drifted")
    expect(integration_desc.get("action") == "validate-performance-governance-integration", "validate-performance-governance-integration description drifted")
    expect(end_to_end_desc.get("action") == "validate-performance-governance-end-to-end", "validate-performance-governance-end-to-end description drifted")
    expect(ci_desc.get("action") == "test-ci", "test-ci description drifted")
    expect(nightly_desc.get("action") == "test-nightly", "test-nightly description drifted")

    expect(
        public_workflow_has_actions(["validate-performance-governance"]),
        "workflow registry no longer references validate-performance-governance",
    )

    render_check = run_capture(python_script_command(PUBLIC_COMMAND_SURFACE_PY, "--check"))
    expect(render_check.returncode == 0, "public command surface check failed")

    hygiene = run_capture(python_script_command(TASK_HYGIENE_PY))
    expect(hygiene.returncode == 0, "task hygiene gate failed")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_report_path": repo_rel(INTEGRATION_REPORT),
        "validate_action": validate_desc["action"],
        "integration_action": integration_desc["action"],
        "end_to_end_action": end_to_end_desc["action"],
        "ci_action": ci_desc["action"],
        "nightly_action": nightly_desc["action"],
        "command_surface_check": "scripts/render_objc3c_public_command_surface.py --check",
        "task_hygiene_gate": "scripts/ci/run_task_hygiene_gate.py",
        "ci_wiring_present": True,
        "nightly_wiring_present": True,
    }
    END_TO_END_REPORT.parent.mkdir(parents=True, exist_ok=True)
    END_TO_END_REPORT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(END_TO_END_REPORT)}")
    print("objc3c-performance-governance-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
