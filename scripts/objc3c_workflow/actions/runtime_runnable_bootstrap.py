"""Runnable bootstrap workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_BOOTSTRAP_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
)


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BOOTSTRAP_E2E_PY)
