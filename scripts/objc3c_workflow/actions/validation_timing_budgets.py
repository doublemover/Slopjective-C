"""Validation timing budget policy helpers."""

from __future__ import annotations

from collections.abc import Sequence

from .validation_timing_numbers import safe_float


VALIDATION_TIMING_BUDGET_OWNER = "validation_timing_budgets"
VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER = "validation_timing_budgets"
DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS = 120.0
COMPOSITE_ELAPSED_THRESHOLD_SECONDS_BY_ACTION = {
    # Public test composites have distinct cold Windows envelopes. The generic
    # fallback remains small for ad-hoc composites; these named gates are the
    # source-owned validation lanes operators actually run.
    "test-smoke": 150.0,
    "test-full": 180.0,
    "test-ci": 3600.0,
    "test-nightly": 21600.0,
    # Release composites include nested native performance, package, and
    # publication gates. Keep these hard-blocking, but budget the cold Windows
    # release lane instead of warmed developer-cache expectations.
    "validate-performance-governance": 3600.0,
    "validate-release-foundation": 3600.0,
    "validate-packaging-channels": 3300.0,
    "validate-release-operations": 3600.0,
    "validate-distribution-credibility": 4200.0,
    "validate-security-hardening": 1800.0,
    # Stress validation intentionally includes fuzz, differential, reducer, and
    # crash-triage children. A cold native build can legitimately dominate the
    # first run, so the hard budget tracks the end-to-end lane instead of a
    # warmed no-op expectation.
    "validate-stress": 900.0,
}
DEFAULT_RUNTIME_ACCEPTANCE_THRESHOLD_SECONDS = 60.0
DEFAULT_EXECUTION_SMOKE_THRESHOLD_SECONDS = 90.0
DEFAULT_EXECUTION_REPLAY_THRESHOLD_SECONDS = 30.0

RUNTIME_ACCEPTANCE_THRESHOLD_SECONDS_BY_ACTION = {
    "test-nightly": 240.0,
}
EXECUTION_SMOKE_THRESHOLD_SECONDS_BY_ACTION = {
    "test-nightly": 150.0,
}
EXECUTION_REPLAY_THRESHOLD_SECONDS_BY_ACTION = {
    "test-nightly": 120.0,
}


def validation_speed_budget_mode() -> str:
    return "fail"


def composite_elapsed_threshold_seconds(composite_action: str | None) -> float:
    if composite_action is None:
        return DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS
    return COMPOSITE_ELAPSED_THRESHOLD_SECONDS_BY_ACTION.get(
        composite_action,
        DEFAULT_COMPOSITE_ELAPSED_THRESHOLD_SECONDS,
    )


def runtime_acceptance_threshold_seconds(composite_action: str | None) -> float:
    if composite_action is None:
        return DEFAULT_RUNTIME_ACCEPTANCE_THRESHOLD_SECONDS
    return RUNTIME_ACCEPTANCE_THRESHOLD_SECONDS_BY_ACTION.get(
        composite_action,
        DEFAULT_RUNTIME_ACCEPTANCE_THRESHOLD_SECONDS,
    )


def execution_smoke_threshold_seconds(composite_action: str | None) -> float:
    if composite_action is None:
        return DEFAULT_EXECUTION_SMOKE_THRESHOLD_SECONDS
    return EXECUTION_SMOKE_THRESHOLD_SECONDS_BY_ACTION.get(
        composite_action,
        DEFAULT_EXECUTION_SMOKE_THRESHOLD_SECONDS,
    )


def execution_replay_threshold_seconds(composite_action: str | None) -> float:
    if composite_action is None:
        return DEFAULT_EXECUTION_REPLAY_THRESHOLD_SECONDS
    return EXECUTION_REPLAY_THRESHOLD_SECONDS_BY_ACTION.get(
        composite_action,
        DEFAULT_EXECUTION_REPLAY_THRESHOLD_SECONDS,
    )


def validation_speed_budgets(
    runtime_acceptance: dict[str, object] | None,
    execution_smoke: dict[str, object] | None,
    execution_replay: dict[str, object] | None,
    total_seconds: float,
    *,
    composite_action: str | None = None,
) -> list[dict[str, object]]:
    budget_mode = validation_speed_budget_mode()
    budgets = [
        {
            "name": "runtime_acceptance_elapsed_seconds",
            "threshold_seconds": runtime_acceptance_threshold_seconds(
                composite_action
            ),
            "actual_seconds": safe_float(
                runtime_acceptance.get("elapsed_seconds") if runtime_acceptance else None
            ),
        },
        {
            "name": "execution_smoke_elapsed_seconds",
            "threshold_seconds": execution_smoke_threshold_seconds(
                composite_action
            ),
            "actual_seconds": safe_float(
                execution_smoke.get("elapsed_seconds") if execution_smoke else None
            ),
        },
        {
            "name": "execution_replay_elapsed_seconds",
            "threshold_seconds": execution_replay_threshold_seconds(
                composite_action
            ),
            "actual_seconds": safe_float(
                execution_replay.get("elapsed_seconds") if execution_replay else None
            ),
        },
        {
            "name": "composite_elapsed_seconds",
            "threshold_seconds": composite_elapsed_threshold_seconds(
                composite_action
            ),
            "actual_seconds": total_seconds,
        },
    ]
    for budget in budgets:
        actual = safe_float(budget["actual_seconds"])
        threshold = safe_float(budget["threshold_seconds"])
        if actual == 0.0:
            budget["status"] = "UNKNOWN"
        elif actual <= threshold:
            budget["status"] = "PASS"
        else:
            budget["status"] = "FAIL"
        budget["mode"] = budget_mode
        budget["budget_owner"] = VALIDATION_TIMING_BUDGET_OWNER
        budget["hard_blocking_decision_owner"] = (
            VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER
        )
    if runtime_acceptance is not None:
        command_groups = runtime_acceptance.get("command_groups", {})
        wrapper_count = None
        if isinstance(command_groups, dict):
            wrapper = command_groups.get("wrapper", {})
            if isinstance(wrapper, dict):
                wrapper_count = wrapper.get("count")
        budgets.append(
            {
                "name": "runtime_acceptance_wrapper_invocations",
                "threshold_count": 1,
                "actual_count": wrapper_count,
                "status": "PASS"
                if isinstance(wrapper_count, int) and wrapper_count <= 1
                else "UNKNOWN"
                if wrapper_count is None
                else "FAIL",
                "mode": budget_mode,
                "budget_owner": VALIDATION_TIMING_BUDGET_OWNER,
                "hard_blocking_decision_owner": (
                    VALIDATION_TIMING_HARD_BLOCKING_DECISION_OWNER
                ),
            }
        )
    return budgets


def validation_budget_violations(
    budgets: Sequence[dict[str, object]],
) -> list[dict[str, object]]:
    return [
        budget
        for budget in budgets
        if budget.get("mode") == "fail" and budget.get("status") == "FAIL"
    ]
