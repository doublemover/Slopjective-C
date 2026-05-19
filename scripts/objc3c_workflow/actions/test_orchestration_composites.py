"""Composite public test orchestration actions."""

from __future__ import annotations

from ..composite_validation import run_composite_validation
from .test_orchestration_profiles import (
    test_orchestration_profile_payload,
    test_orchestration_steps,
)


def _run_profile(action: str) -> int:
    test_orchestration_profile_payload(action)
    return run_composite_validation(action, test_orchestration_steps(action))


def action_test_smoke(_: list[str]) -> int:
    return _run_profile("test-smoke")


def action_test_ci(_: list[str]) -> int:
    return _run_profile("test-ci")


def action_test_full(_: list[str]) -> int:
    return _run_profile("test-full")


def action_test_nightly(_: list[str]) -> int:
    return _run_profile("test-nightly")
