"""Validation timing child-report aggregation helpers."""

from __future__ import annotations

from collections.abc import Sequence

from .validation_timing_budgets import validation_speed_budgets
from .validation_timing_child_report_loading import load_child_reports
from .validation_timing_numbers import safe_float
from .validation_timing_report_summaries import (
    summarize_execution_replay_report,
    summarize_execution_smoke_report,
    summarize_runtime_acceptance_report,
)


def collect_child_timing(
    action: str,
    steps: Sequence[dict[str, object]]
) -> dict[str, object]:
    child_reports = load_child_reports(steps)
    runtime_acceptance: dict[str, object] | None = None
    execution_smoke: dict[str, object] | None = None
    execution_replay: dict[str, object] | None = None
    for report in child_reports:
        payload = report["payload"]
        if not isinstance(payload, dict):
            continue
        path = str(report["path"])
        if "default_compile_backend" in payload or "case_count" in payload:
            runtime_acceptance = summarize_runtime_acceptance_report(
                path,
                payload,
                report_reused=bool(report.get("report_reused", False)),
            )
        elif "proof_run_id" in payload:
            execution_replay = summarize_execution_replay_report(path, payload)
        elif "compile_command" in payload and "results" in payload:
            execution_smoke = summarize_execution_smoke_report(path, payload)
    total_step_seconds = round(
        sum(safe_float(step.get("duration_seconds", 0.0)) for step in steps),
        6,
    )
    estimated_no_skip_seconds = total_step_seconds
    if runtime_acceptance is not None and runtime_acceptance.get("report_reused"):
        for step in steps:
            if step.get("action") == "test-runtime-acceptance":
                estimated_no_skip_seconds -= safe_float(
                    step.get("duration_seconds", 0.0)
                )
                estimated_no_skip_seconds += safe_float(
                    runtime_acceptance.get("elapsed_seconds", 0.0)
                )
                break
    estimated_no_skip_seconds = round(estimated_no_skip_seconds, 6)
    return {
        "child_report_paths": [str(report["path"]) for report in child_reports],
        "runtime_acceptance": runtime_acceptance,
        "execution_smoke": execution_smoke,
        "execution_replay": execution_replay,
        "estimated_no_skip_seconds": estimated_no_skip_seconds,
        "budgets": validation_speed_budgets(
            runtime_acceptance,
            execution_smoke,
            execution_replay,
            estimated_no_skip_seconds,
            composite_action=action,
        ),
    }
