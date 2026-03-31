#!/usr/bin/env python3
"""Validate runtime hardening and memory-safety regression evidence for security hardening."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "runtime_hardening_contract.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "runtime-hardening-summary.json"
ACTION_REPORTS = {
    "test-runtime-acceptance": [
        ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json",
    ],
    "validate-runnable-release-candidate": [
        ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-e2e" / "summary.json",
    ],
    "validate-release-candidate-conformance": [
        ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-conformance" / "summary.json",
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
        print(f"security-hardening-runtime-hardening: FAIL\n- {exc}", file=sys.stderr)
        return 1

    failures: list[str] = []
    reports: dict[str, dict[str, Any]] = {}
    for raw_path in contract["required_reports"]:
        path = ROOT / str(raw_path)
        if not path.is_file():
            failures.append(f"missing required report {raw_path}")
            continue
        payload = load_json(path)
        reports[str(raw_path)] = payload
        if payload.get("status") not in {"PASS", "OK"} and payload.get("ok") is not True:
            failures.append(f"required report did not pass: {raw_path}")

    runtime_acceptance = reports.get("tmp/reports/runtime/acceptance/summary.json", {})
    runnable_release_candidate = reports.get("tmp/reports/runtime/runnable-release-candidate-e2e/summary.json", {})
    release_candidate_conformance = reports.get("tmp/reports/runtime/runnable-release-candidate-conformance/summary.json", {})

    runtime_case_ids = {
        str(case.get("case_id"))
        for case in runtime_acceptance.get("cases", [])
        if isinstance(case, dict) and case.get("passed") is True
    }
    missing_case_ids = [
        case_id for case_id in contract["required_runtime_case_ids"] if str(case_id) not in runtime_case_ids
    ]
    if missing_case_ids:
        failures.append(f"missing required runtime case ids: {', '.join(missing_case_ids)}")

    packaged_artifacts = runnable_release_candidate.get("packaged_validate_artifacts", [])
    if len(packaged_artifacts) < 5:
        failures.append("runnable release-candidate validation artifact set was incomplete")

    child_reports = release_candidate_conformance.get("child_report_paths", [])
    if "tmp/reports/runtime/acceptance/summary.json" not in child_reports:
        failures.append("release-candidate conformance no longer references runtime acceptance")

    payload = {
        "contract_id": "objc3c.security.hardening.runtime.hardening.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "required_validate_actions": contract["required_validate_actions"],
        "reused_validate_actions": reused_actions,
        "executed_validate_actions": executed_actions,
        "required_reports": contract["required_reports"],
        "runtime_case_count": runtime_acceptance.get("case_count"),
        "required_runtime_case_count": len(contract["required_runtime_case_ids"]),
        "missing_runtime_case_ids": missing_case_ids,
        "packaged_validate_artifact_count": len(packaged_artifacts) if isinstance(packaged_artifacts, list) else 0,
        "release_candidate_required_case_count": len(release_candidate_conformance.get("required_case_ids", []))
        if isinstance(release_candidate_conformance.get("required_case_ids", []), list)
        else 0,
        "memory_safety_boundary": "bounded-by-runtime-acceptance-and-packaged-release-candidate-evidence",
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("security-hardening-runtime-hardening: PASS" if not failures else "security-hardening-runtime-hardening: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
