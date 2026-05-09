"""Runnable storage/reflection workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
)
RUNNABLE_STORAGE_REFLECTION_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
)


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_E2E_PY)


__all__ = [
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY",
    "RUNNABLE_STORAGE_REFLECTION_E2E_PY",
    "action_validate_storage_reflection_conformance",
    "action_validate_runnable_storage_reflection",
]
