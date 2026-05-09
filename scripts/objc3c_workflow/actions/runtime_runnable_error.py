"""Runnable error workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_ERROR_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_error_conformance.py"
)
RUNNABLE_ERROR_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_error_end_to_end.py"


def action_validate_error_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_CONFORMANCE_PY)


def action_validate_runnable_error(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_E2E_PY)
