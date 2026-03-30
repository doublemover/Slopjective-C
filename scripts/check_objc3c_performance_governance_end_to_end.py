#!/usr/bin/env python3
"""Validate the performance-governance workflow end to end."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PUBLIC_COMMAND_SURFACE_PY = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "performance-governance" / "integration-summary.json"
END_TO_END_REPORT = ROOT / "tmp" / "reports" / "performance-governance" / "end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.performance.governance.end_to_end.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


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
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def ensure_integration_report() -> dict[str, Any]:
    if INTEGRATION_REPORT.is_file():
        report = load_json(INTEGRATION_REPORT)
        if report.get("status") == "PASS":
            return report
    completed = run_capture([sys.executable, str(RUNNER), "validate-performance-governance-integration"])
    expect(completed.returncode == 0, "validate-performance-governance-integration failed during end-to-end validation")
    return load_json(INTEGRATION_REPORT)


def describe_action(action: str) -> dict[str, Any]:
    completed = run_capture([sys.executable, str(RUNNER), "--describe", action])
    expect(completed.returncode == 0, f"--describe {action} failed")
    payload = json.loads(completed.stdout)
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

    expect(validate_desc.get("public_scripts") == ["test:objc3c:performance-governance"], "validate-performance-governance public script drifted")
    expect(integration_desc.get("public_scripts") == ["test:objc3c:performance-governance:integration"], "validate-performance-governance-integration public script drifted")
    expect(end_to_end_desc.get("public_scripts") == ["test:objc3c:performance-governance:e2e"], "validate-performance-governance-end-to-end public script drifted")
    expect(ci_desc.get("action") == "test-ci", "test-ci description drifted")
    expect(nightly_desc.get("action") == "test-nightly", "test-nightly description drifted")

    runner_source = RUNNER.read_text(encoding="utf-8")
    expect('"validate-performance-governance"' in runner_source, "runner no longer references validate-performance-governance")

    render_check = run_capture([sys.executable, str(PUBLIC_COMMAND_SURFACE_PY), "--check"])
    expect(render_check.returncode == 0, "public command surface check failed")

    hygiene = run_capture([sys.executable, str(TASK_HYGIENE_PY)])
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
