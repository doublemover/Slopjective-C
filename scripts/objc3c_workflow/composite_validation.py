"""Composite public workflow execution."""

from __future__ import annotations

from collections.abc import Sequence
from time import perf_counter

from .composite_progress import (
    print_composite_step_done,
    print_composite_step_start,
)
from .composite_report_finalization import (
    composite_report_failed,
    write_and_announce_composite_report,
)
from .composite_steps import run_composite_step


def run_composite_validation(
    action: str,
    steps: list[tuple[str, Sequence[str]]],
    *,
    continue_on_failure: bool = False,
) -> int:
    results: list[dict[str, object]] = []
    first_failure_code = 0
    workflow_started_at = perf_counter()
    for index, (step_action, command) in enumerate(steps, start=1):
        previous = str(results[-1]["action"]) if results else "none"
        print_composite_step_start(
            index=index,
            total=len(steps),
            action=step_action,
            previous_action=previous,
            workflow_started_at=workflow_started_at,
        )
        step = run_composite_step(step_action, command)
        results.append(step)
        print_composite_step_done(
            index=index,
            total=len(steps),
            action=step_action,
            step=step,
            workflow_started_at=workflow_started_at,
        )
        if step["exit_code"] != 0:
            if first_failure_code == 0:
                first_failure_code = int(step["exit_code"])
            if not continue_on_failure:
                write_and_announce_composite_report(
                    action, results, status="FAIL"
                )
                return first_failure_code
    report_status = "FAIL" if first_failure_code else "PASS"
    report_path = write_and_announce_composite_report(
        action,
        results,
        status=report_status,
    )
    if first_failure_code:
        return first_failure_code
    if composite_report_failed(report_path):
        return 1
    return 0
