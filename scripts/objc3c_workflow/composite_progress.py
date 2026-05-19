"""Progress output for composite public workflow execution."""

from __future__ import annotations

from time import perf_counter

from .composite_report_policy import (
    composite_progress_done_line,
    composite_progress_start_line,
)


def print_composite_step_start(
    *,
    index: int,
    total: int,
    action: str,
    previous_action: str,
    workflow_started_at: float,
) -> None:
    print(
        composite_progress_start_line(
            index=index,
            total=total,
            action=action,
            previous_action=previous_action,
            elapsed_seconds=perf_counter() - workflow_started_at,
        ),
        flush=True,
    )


def print_composite_step_done(
    *,
    index: int,
    total: int,
    action: str,
    step: dict[str, object],
    workflow_started_at: float,
) -> None:
    print(
        composite_progress_done_line(
            index=index,
            total=total,
            action=action,
            duration_seconds=float(step.get("duration_seconds", 0.0)),
            elapsed_seconds=perf_counter() - workflow_started_at,
            exit_code=step["exit_code"],
        ),
        flush=True,
    )


__all__ = ["print_composite_step_done", "print_composite_step_start"]
