#!/usr/bin/env python3
"""Validate the public end-to-end stress workflow surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "stress" / "integration-summary.json"
COMMAND_SURFACE = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
PACKAGE_JSON = ROOT / "package.json"
RENDER_COMMAND_SURFACE = ROOT / "scripts" / "render_objc3c_public_command_surface.py"
TASK_HYGIENE_GATE = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "stress" / "end-to-end-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.end-to-end.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def run_checked(command: list[str], message: str) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    expect(completed.returncode == 0, message)
    return completed


def ensure_integration_report() -> dict[str, Any]:
    if INTEGRATION_REPORT.is_file():
        report = load_json(INTEGRATION_REPORT)
        if report.get("status") == "PASS":
            return report
    run_checked(
        [sys.executable, str(ROOT / "scripts" / "check_objc3c_stress_integration.py")],
        "stress integration validator failed during end-to-end validation",
    )
    return load_json(INTEGRATION_REPORT)


def main() -> int:
    integration_report = ensure_integration_report()
    expect(integration_report.get("status") == "PASS", "stress integration report did not pass")

    describe = run_checked(
        [sys.executable, str(RUNNER), "--describe", "test-nightly"],
        "failed to describe test-nightly",
    )
    nightly_payload = json.loads(describe.stdout)
    expect(nightly_payload.get("action") == "test-nightly", "test-nightly describe payload drifted")
    expect(nightly_payload.get("validation_tier") == "nightly", "test-nightly validation_tier drifted")

    package_payload = load_json(PACKAGE_JSON)
    scripts = package_payload.get("scripts", {})
    expect(isinstance(scripts, dict), "package.json scripts drifted from object form")
    for script_name in (
        "check:stress:surface",
        "test:objc3c:stress",
        "test:objc3c:stress:integration",
        "test:objc3c:stress:e2e",
    ):
        expect(script_name in scripts, f"package.json missing {script_name}")

    runner_text = RUNNER.read_text(encoding="utf-8")
    nightly_block = runner_text.partition("def action_test_nightly(_: list[str]) -> int:")[2].partition(
        "\n\ndef action_package_runnable_toolchain"
    )[0]
    expect(nightly_block, "failed to locate action_test_nightly definition in public runner")
    expect('"validate-stress"' in nightly_block, "test-nightly no longer wires validate-stress into the nightly composite")

    run_checked(
        [sys.executable, str(RENDER_COMMAND_SURFACE), "--check"],
        "public command surface check failed during stress end-to-end validation",
    )
    run_checked(
        [sys.executable, str(TASK_HYGIENE_GATE)],
        "task hygiene gate failed during stress end-to-end validation",
    )

    command_surface_text = COMMAND_SURFACE.read_text(encoding="utf-8")
    expect("test:objc3c:stress" in command_surface_text, "public command surface is missing test:objc3c:stress")
    expect(
        "test:objc3c:stress:integration" in command_surface_text,
        "public command surface is missing test:objc3c:stress:integration",
    )
    expect("test:objc3c:stress:e2e" in command_surface_text, "public command surface is missing test:objc3c:stress:e2e")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_report_path": repo_rel(INTEGRATION_REPORT),
        "nightly_description_action": nightly_payload["action"],
        "nightly_validation_tier": nightly_payload["validation_tier"],
        "nightly_includes_validate_stress": True,
        "command_surface_path": repo_rel(COMMAND_SURFACE),
        "required_package_scripts": [
            "check:stress:surface",
            "test:objc3c:stress",
            "test:objc3c:stress:integration",
            "test:objc3c:stress:e2e",
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-stress-end-to-end: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
