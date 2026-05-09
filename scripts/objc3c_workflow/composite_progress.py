"""Progress output for composite public workflow execution."""

from __future__ import annotations

from time import perf_counter


def print_composite_step_start(
    *,
    index: int,
    total: int,
    action: str,
    previous_action: str,
    workflow_started_at: float,
) -> None:
    print(
        f"public-workflow-progress: [{index}/{total}] START action={action} "
        f"elapsed={perf_counter() - workflow_started_at:.3f}s last={previous_action}",
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
        f"public-workflow-progress: [{index}/{total}] DONE action={action} "
        f"duration={float(step.get('duration_seconds', 0.0)):.3f}s "
        f"elapsed={perf_counter() - workflow_started_at:.3f}s exit={step['exit_code']}",
        flush=True,
    )


__all__ = ["print_composite_step_done", "print_composite_step_start"]
