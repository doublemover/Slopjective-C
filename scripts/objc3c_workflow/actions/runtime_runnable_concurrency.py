"""Runnable concurrency workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_CONCURRENCY_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_conformance.py"
)
RUNNABLE_CONCURRENCY_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_end_to_end.py"
)


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_E2E_PY)
