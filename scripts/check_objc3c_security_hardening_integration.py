#!/usr/bin/env python3
"""Validate the integrated security-hardening workflow and publication outputs."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
WORKFLOW_REPORT = ROOT / "tmp" / "reports" / "objc3c-public-workflow" / "validate-security-hardening.json"
RESPONSE_DRILL_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "response-drill-summary.json"
RUNTIME_HARDENING_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "runtime-hardening-summary.json"
POSTURE_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "security-posture-summary.json"
PUBLICATION_SUMMARY = ROOT / "tmp" / "reports" / "security-hardening" / "publication-summary.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "integration-summary.json"

REQUIRED_STEPS = [
    "check-security-response-drill",
    "check-security-runtime-hardening",
    "check-security-hardening-surface",
    "check-security-hardening-schema-surface",
    "build-security-posture",
    "publish-security-advisories",
]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def ensure_workflow_report() -> dict[str, Any]:
    completed = subprocess.run(
        [sys.executable, str(RUNNER), "validate-security-hardening"],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if completed.stdout:
        sys.stdout.write(completed.stdout)
    if completed.stderr:
        sys.stderr.write(completed.stderr)
    if completed.returncode != 0:
        raise RuntimeError("validate-security-hardening command failed during integration validation")
    return load_json(WORKFLOW_REPORT)


def main() -> int:
    try:
        workflow_report = ensure_workflow_report()
    except RuntimeError as exc:
        print(f"objc3c-security-hardening-integration: FAIL\n- {exc}", file=sys.stderr)
        return 1

    steps = workflow_report.get("steps", [])
    if not isinstance(steps, list):
        print("objc3c-security-hardening-integration: FAIL\n- workflow report steps drifted", file=sys.stderr)
        return 1
    step_actions = [str(step.get("action")) for step in steps if isinstance(step, dict)]
    if step_actions != REQUIRED_STEPS:
        print(
            f"objc3c-security-hardening-integration: FAIL\n- workflow steps drifted: {step_actions}",
            file=sys.stderr,
        )
        return 1

    for path in (RESPONSE_DRILL_SUMMARY, RUNTIME_HARDENING_SUMMARY, POSTURE_SUMMARY, PUBLICATION_SUMMARY):
        if not path.is_file():
            print(f"objc3c-security-hardening-integration: FAIL\n- missing {repo_rel(path)}", file=sys.stderr)
            return 1

    response_drill = load_json(RESPONSE_DRILL_SUMMARY)
    runtime_hardening = load_json(RUNTIME_HARDENING_SUMMARY)
    posture = load_json(POSTURE_SUMMARY)
    publication = load_json(PUBLICATION_SUMMARY)
    if response_drill.get("status") != "PASS":
        print("objc3c-security-hardening-integration: FAIL\n- response drill did not pass", file=sys.stderr)
        return 1
    if runtime_hardening.get("status") != "PASS":
        print("objc3c-security-hardening-integration: FAIL\n- runtime hardening did not pass", file=sys.stderr)
        return 1
    if posture.get("status") != "PASS" or publication.get("status") != "PASS":
        print("objc3c-security-hardening-integration: FAIL\n- publication artifacts did not pass", file=sys.stderr)
        return 1
    if posture.get("security_state") != publication.get("security_state"):
        print("objc3c-security-hardening-integration: FAIL\n- posture/publication state drifted", file=sys.stderr)
        return 1

    payload = {
        "contract_id": "objc3c.security.hardening.integration.summary.v1",
        "status": "PASS",
        "workflow_report_path": repo_rel(WORKFLOW_REPORT),
        "validated_steps": REQUIRED_STEPS,
        "response_drill_summary_path": repo_rel(RESPONSE_DRILL_SUMMARY),
        "runtime_hardening_summary_path": repo_rel(RUNTIME_HARDENING_SUMMARY),
        "posture_summary_path": repo_rel(POSTURE_SUMMARY),
        "publication_summary_path": repo_rel(PUBLICATION_SUMMARY),
        "security_state": publication.get("security_state"),
        "response_trust_state": response_drill.get("trust_state"),
        "runtime_memory_safety_boundary": runtime_hardening.get("memory_safety_boundary"),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-security-hardening-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
