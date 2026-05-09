"""Runnable metaprogramming workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
)
RUNNABLE_METAPROGRAMMING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_end_to_end.py"
)


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_E2E_PY)


__all__ = [
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY",
    "RUNNABLE_METAPROGRAMMING_E2E_PY",
    "action_validate_metaprogramming_conformance",
    "action_validate_runnable_metaprogramming",
]
