#!/usr/bin/env python3
"""Run the checked-in security response drill over the live release and trust surfaces."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "response_drill_contract.json"
RESPONSE_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "security_response_disclosure_policy.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "response-drill-summary.json"
TRUST_REPORT = ROOT / "tmp" / "artifacts" / "distribution-credibility" / "report" / "objc3c-distribution-trust-report.json"
ACTION_REPORTS = {
    "validate-platform-hardening": [
        ROOT / "tmp" / "reports" / "platform-hardening" / "integration-summary.json",
    ],
    "validate-distribution-credibility": [
        ROOT / "tmp" / "reports" / "distribution-credibility" / "publication-summary.json",
    ],
    "validate-distribution-credibility-end-to-end": [
        ROOT / "tmp" / "reports" / "distribution-credibility" / "end-to-end-summary.json",
    ],
}


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(list(command), cwd=ROOT, check=False, text=True, capture_output=True)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def report_passes(path: Path) -> bool:
    if not path.is_file():
        return False
    payload = load_json(path)
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def ensure_action(action: str) -> bool:
    report_paths = ACTION_REPORTS.get(action, [])
    if report_paths and all(report_passes(path) for path in report_paths):
        return True
    result = run_capture([sys.executable, str(RUNNER), action])
    if result.returncode != 0:
        raise RuntimeError(f"{action} failed")
    return False


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    response_policy = load_json(RESPONSE_POLICY)
    reused_actions: list[str] = []
    executed_actions: list[str] = []

    try:
        for action in contract["required_validate_actions"]:
            action_name = str(action)
            if ensure_action(action_name):
                reused_actions.append(action_name)
            else:
                executed_actions.append(action_name)
    except RuntimeError as exc:
        print(f"security-hardening-response-drill: FAIL\n- {exc}", file=sys.stderr)
        return 1

    failures: list[str] = []
    for raw_path in contract["required_reports"]:
        path = ROOT / str(raw_path)
        if not path.is_file():
            failures.append(f"missing required report {raw_path}")
            continue
        payload = load_json(path)
        if payload.get("status") not in {"PASS", "OK"} and payload.get("ok") is not True:
            failures.append(f"required report did not pass: {raw_path}")

    for raw_path in contract["required_artifacts"]:
        path = ROOT / str(raw_path)
        if not path.is_file():
            failures.append(f"missing required artifact {raw_path}")

    trust_report = load_json(TRUST_REPORT)
    trust_state = str(trust_report.get("trust_state", "blocked"))
    if trust_state == "blocked":
        failures.append("distribution trust report is blocked")

    payload = {
        "contract_id": "objc3c.security.hardening.response.drill.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "required_validate_actions": contract["required_validate_actions"],
        "reused_validate_actions": reused_actions,
        "executed_validate_actions": executed_actions,
        "required_reports": contract["required_reports"],
        "required_artifacts": contract["required_artifacts"],
        "trust_state": trust_state,
        "incident_class_count": len(response_policy["incident_classes"]),
        "blocking_condition_count": len(response_policy["blocking_conditions"]),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-hardening-response-drill: PASS" if not failures else "security-hardening-response-drill: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
