"""Stress and fuzz workflow actions."""

from __future__ import annotations

from ..registry_views import actions_matching
from .stress_catalog import (
    FUZZ_SAFETY_PY,
    LOWERING_RUNTIME_STRESS_PY,
    MIXED_MODULE_DIFFERENTIAL_PY,
    STRESS_CRASH_TRIAGE_PY,
    STRESS_END_TO_END_PY,
    STRESS_INTEGRATION_PY,
    STRESS_MINIMIZATION_PY,
    STRESS_SCENARIOS,
    STRESS_SOURCE_SURFACE_PY,
    StressScenario,
)
from .stress_execution import (
    run_stress_scenario,
    run_validate_stress,
    run_validate_stress_end_to_end,
    run_validate_stress_integration,
)


def action_names() -> list[str]:
    return actions_matching(
        lambda action, _: "stress" in action or "fuzz" in action
    )


def action_check_stress_surface(_: list[str]) -> int:
    return run_stress_scenario("check-stress-surface")


def action_test_fuzz_safety(rest: list[str]) -> int:
    return run_stress_scenario("test-fuzz-safety", rest)


def action_test_lowering_runtime_stress(rest: list[str]) -> int:
    return run_stress_scenario("test-lowering-runtime-stress", rest)


def action_test_mixed_module_differential(rest: list[str]) -> int:
    return run_stress_scenario("test-mixed-module-differential", rest)


def action_test_stress_minimization(rest: list[str]) -> int:
    return run_stress_scenario("test-stress-minimization", rest)


def action_test_stress_crash_triage(rest: list[str]) -> int:
    return run_stress_scenario("test-stress-crash-triage", rest)


def action_validate_stress(_: list[str]) -> int:
    return run_validate_stress()


def action_validate_stress_integration(_: list[str]) -> int:
    return run_validate_stress_integration()


def action_validate_stress_end_to_end(_: list[str]) -> int:
    return run_validate_stress_end_to_end()


__all__ = [
    "FUZZ_SAFETY_PY",
    "LOWERING_RUNTIME_STRESS_PY",
    "MIXED_MODULE_DIFFERENTIAL_PY",
    "STRESS_CRASH_TRIAGE_PY",
    "STRESS_END_TO_END_PY",
    "STRESS_INTEGRATION_PY",
    "STRESS_MINIMIZATION_PY",
    "STRESS_SCENARIOS",
    "STRESS_SOURCE_SURFACE_PY",
    "StressScenario",
    "action_check_stress_surface",
    "action_names",
    "action_test_fuzz_safety",
    "action_test_lowering_runtime_stress",
    "action_test_mixed_module_differential",
    "action_test_stress_crash_triage",
    "action_test_stress_minimization",
    "action_validate_stress",
    "action_validate_stress_end_to_end",
    "action_validate_stress_integration",
]
