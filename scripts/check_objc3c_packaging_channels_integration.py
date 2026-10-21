#!/usr/bin/env python3
"""Validate the integrated objc3c packaging-channels workflow report."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture
from scripts.objc3c_workflow.public_command_api import public_workflow_command

ROOT = Path(__file__).resolve().parents[1]
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "workflow_surface.json"
PUBLIC_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-channels" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.packaging.channels.integration.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--use-existing-validate-report",
        action="store_true",
        help=(
            "validate the already-produced validate-packaging-channels public "
            "workflow report instead of invoking the public workflow command"
        ),
    )
    return parser.parse_args()


def workflow_report_path(validate_action: str) -> Path:
    return PUBLIC_REPORT_ROOT / f"{validate_action}.json"


def expected_step_order(workflow_surface: dict[str, Any]) -> list[str]:
    steps = workflow_surface.get("integrated_required_steps")
    expect(
        isinstance(steps, list) and all(isinstance(step, str) for step in steps),
        "packaging-channels workflow surface missing integrated_required_steps",
    )
    return list(steps)


def ensure_validate_report(validate_action: str, *, use_existing: bool) -> tuple[Path, dict[str, Any]]:
    report_path = workflow_report_path(validate_action)
    if not use_existing:
        completed = run_capture(public_workflow_command(validate_action))
        expect(
            completed.returncode == 0,
            f"{validate_action} command failed during packaging-channels integration validation",
        )
    else:
        expect(
            report_path.is_file(),
            (
                f"{validate_action} workflow report is missing for existing-report "
                "packaging-channels integration validation"
            ),
        )
    return report_path, load_json(report_path)


def failing_step_summary(steps: Any) -> str:
    if not isinstance(steps, list):
        return "steps were not published"
    for step in steps:
        if not isinstance(step, dict):
            return "steps contained a non-object entry"
        if step.get("exit_code") != 0:
            return f"first failing step {step.get('action')!r} exited {step.get('exit_code')!r}"
    return "no failing step was published"


def validate_public_workflow_report(
    *,
    report: dict[str, Any],
    workflow_surface: dict[str, Any],
    report_path: Path,
) -> list[str]:
    validate_action = workflow_surface.get("validate_action")
    expect(
        report.get("action") == validate_action,
        "public packaging-channels report drifted from workflow_surface validate_action",
    )
    status = report.get("status")
    expect(
        status == "PASS",
        (
            f"public packaging-channels report is not PASS ({status!r}); "
            f"regenerate {repo_rel(report_path)} with "
            f"npm run objc3c -- {validate_action}; {failing_step_summary(report.get('steps'))}"
        ),
    )
    steps = report.get("steps")
    expect(isinstance(steps, list), "public packaging-channels report did not publish steps")
    expect(
        all(isinstance(step, dict) for step in steps),
        "public packaging-channels report contained a non-object step",
    )
    step_actions = [str(step.get("action")) for step in steps]
    expected_steps = expected_step_order(workflow_surface)
    expect(
        step_actions == expected_steps,
        f"packaging-channels step order drifted: {step_actions!r}",
    )
    for step in steps:
        expect(
            step.get("exit_code") == 0,
            (
                f"packaging-channels step {step.get('action')!r} did not pass "
                f"(exit_code={step.get('exit_code')!r})"
            ),
        )
    return step_actions


def main() -> int:
    args = parse_args()
    workflow_surface = load_json(WORKFLOW_SURFACE)
    owner_policy = workflow_surface.get("owner_policy")
    expect(
        isinstance(owner_policy, dict) and owner_policy.get("evidence_log_allowed") is False,
        "packaging-channels workflow surface missing source-owned owner_policy",
    )
    blocker_metadata = workflow_surface.get("blocker_metadata")
    expect(
        isinstance(blocker_metadata, dict)
        and blocker_metadata.get("blocker_owner") == "packaging-channels-blockers",
        "packaging-channels workflow surface missing blocker metadata",
    )
    validate_action = str(workflow_surface.get("validate_action"))
    report_path, report = ensure_validate_report(
        validate_action,
        use_existing=bool(args.use_existing_validate_report),
    )
    step_actions = validate_public_workflow_report(
        report=report,
        workflow_surface=workflow_surface,
        report_path=report_path,
    )

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "workflow_surface": repo_rel(WORKFLOW_SURFACE),
        "public_workflow_report": repo_rel(report_path),
        "public_workflow_report_status": report.get("status"),
        "public_workflow_report_generated_at_utc": report.get("generated_at_utc"),
        "used_existing_validate_report": bool(args.use_existing_validate_report),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "step_order": step_actions,
        "stale_failure_reports_allowed": False,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-packaging-channels-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
