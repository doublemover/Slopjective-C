"""Runnable object-model workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
)
RUNNABLE_OBJECT_MODEL_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
)


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_E2E_PY)


__all__ = [
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY",
    "RUNNABLE_OBJECT_MODEL_E2E_PY",
    "action_validate_object_model_conformance",
    "action_validate_runnable_object_model",
]
