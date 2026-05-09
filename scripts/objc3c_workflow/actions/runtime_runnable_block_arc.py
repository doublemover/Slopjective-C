"""Runnable block/ARC workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_BLOCK_ARC_ROUTE = "block-arc"
VALIDATE_BLOCK_ARC_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_BLOCK_ARC_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_BLOCK_ARC_ACTION = f"validate-runnable-{RUNNABLE_BLOCK_ARC_ROUTE}"

RUNNABLE_BLOCK_ARC_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_block_arc_conformance.py"
)
RUNNABLE_BLOCK_ARC_E2E_SCRIPT = "scripts/check_objc3c_runnable_block_arc_end_to_end.py"
RUNNABLE_BLOCK_ARC_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_BLOCK_ARC_CONFORMANCE_SCRIPT}"
)
RUNNABLE_BLOCK_ARC_E2E_BACKEND = f"python:{RUNNABLE_BLOCK_ARC_E2E_SCRIPT}"
RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = ROOT / RUNNABLE_BLOCK_ARC_CONFORMANCE_SCRIPT
RUNNABLE_BLOCK_ARC_E2E_PY = ROOT / RUNNABLE_BLOCK_ARC_E2E_SCRIPT


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_E2E_PY)


RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_BLOCK_ARC_CONFORMANCE_ACTION,
    summary="validate runnable block/ARC conformance across the integrated live workflow",
    backend=RUNNABLE_BLOCK_ARC_CONFORMANCE_BACKEND,
    handler=action_validate_block_arc_conformance,
    guarantee_owner=(
        "integrated block/ARC conformance over the live runtime architecture workflow"
    ),
)
RUNNABLE_BLOCK_ARC_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_BLOCK_ARC_ACTION,
    summary="validate runnable block/ARC execution end to end from the package root",
    backend=RUNNABLE_BLOCK_ARC_E2E_BACKEND,
    handler=action_validate_runnable_block_arc,
    guarantee_owner=(
        "packaged compile, block/ARC probe execution, smoke, and replay from the "
        "staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION",
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_BACKEND",
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_PY",
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_SCRIPT",
    "RUNNABLE_BLOCK_ARC_E2E_ACTION",
    "RUNNABLE_BLOCK_ARC_E2E_BACKEND",
    "RUNNABLE_BLOCK_ARC_E2E_PY",
    "RUNNABLE_BLOCK_ARC_E2E_SCRIPT",
    "RUNNABLE_BLOCK_ARC_ROUTE",
    "VALIDATE_BLOCK_ARC_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_BLOCK_ARC_ACTION",
    "action_validate_block_arc_conformance",
    "action_validate_runnable_block_arc",
]
