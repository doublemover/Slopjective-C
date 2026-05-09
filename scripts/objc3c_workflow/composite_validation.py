"""Composite public workflow execution."""

from __future__ import annotations

from collections.abc import Sequence
from time import perf_counter

from .actions.validation_timing import load_latest_report_payload
from .composite_reports import write_composite_validation_report
from .composite_steps import run_composite_step
from .environment import ROOT


def run_composite_validation(action: str, steps: list[tuple[str, Sequence[str]]]) -> int:
    results: list[dict[str, object]] = []
    workflow_started_at = perf_counter()
    for index, (step_action, command) in enumerate(steps, start=1):
        previous = str(results[-1]["action"]) if results else "none"
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] START action={step_action} "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s last={previous}",
            flush=True,
        )
        step = run_composite_step(step_action, command)
        results.append(step)
        print(
            f"public-workflow-progress: [{index}/{len(steps)}] DONE action={step_action} "
            f"duration={float(step.get('duration_seconds', 0.0)):.3f}s "
            f"elapsed={perf_counter() - workflow_started_at:.3f}s exit={step['exit_code']}",
            flush=True,
        )
        if step["exit_code"] != 0:
            report_path = write_composite_validation_report(
                action, results, status="FAIL"
            )
            print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
            return int(step["exit_code"])
    report_path = write_composite_validation_report(action, results, status="PASS")
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")
    report_payload = load_latest_report_payload(report_path)
    if isinstance(report_payload, dict) and report_payload.get("status") != "PASS":
        return 1
    return 0
