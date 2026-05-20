"""Stress command execution and report orchestration."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from .stress_catalog import (
    STRESS_END_TO_END_PY,
    STRESS_INTEGRATION_PY,
    STRESS_SCENARIOS,
    VALIDATE_STRESS_CHILD_ACTIONS,
)


def stress_command(action_name: str, rest: list[str] | None = None) -> list[str]:
    scenario = STRESS_SCENARIOS[action_name]
    return [sys.executable, str(scenario.script), *(rest or [])]


def run_stress_scenario(action_name: str, rest: list[str] | None = None) -> int:
    return run(stress_command(action_name, rest))


def run_validate_stress() -> int:
    return run_composite_validation(
        "validate-stress",
        [
            (action_name, stress_command(action_name))
            for action_name in VALIDATE_STRESS_CHILD_ACTIONS
        ],
        continue_on_failure=True,
    )


def run_validate_stress_integration() -> int:
    return run([sys.executable, str(STRESS_INTEGRATION_PY)])


def run_validate_stress_end_to_end() -> int:
    return run([sys.executable, str(STRESS_END_TO_END_PY)])
