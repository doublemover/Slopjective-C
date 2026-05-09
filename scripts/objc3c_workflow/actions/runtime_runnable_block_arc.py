"""Runnable block/ARC workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
)
RUNNABLE_BLOCK_ARC_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_end_to_end.py"
)


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_E2E_PY)
