"""Runnable interop workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_INTEROP_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_interop_conformance.py"
)
RUNNABLE_INTEROP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_end_to_end.py"


def action_validate_interop_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_CONFORMANCE_PY)


def action_validate_runnable_interop(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_E2E_PY)


__all__ = [
    "RUNNABLE_INTEROP_CONFORMANCE_PY",
    "RUNNABLE_INTEROP_E2E_PY",
    "action_validate_interop_conformance",
    "action_validate_runnable_interop",
]
