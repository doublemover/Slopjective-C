"""Runnable concurrency workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_CONCURRENCY_ROUTE = "concurrency"
VALIDATE_CONCURRENCY_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_CONCURRENCY_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_CONCURRENCY_ACTION = (
    f"validate-runnable-{RUNNABLE_CONCURRENCY_ROUTE}"
)

RUNNABLE_CONCURRENCY_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_concurrency_conformance.py"
)
RUNNABLE_CONCURRENCY_E2E_SCRIPT = (
    "scripts/check_objc3c_runnable_concurrency_end_to_end.py"
)
RUNNABLE_CONCURRENCY_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_CONCURRENCY_CONFORMANCE_SCRIPT}"
)
RUNNABLE_CONCURRENCY_E2E_BACKEND = f"python:{RUNNABLE_CONCURRENCY_E2E_SCRIPT}"
RUNNABLE_CONCURRENCY_CONFORMANCE_PY = ROOT / RUNNABLE_CONCURRENCY_CONFORMANCE_SCRIPT
RUNNABLE_CONCURRENCY_E2E_PY = ROOT / RUNNABLE_CONCURRENCY_E2E_SCRIPT


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_E2E_PY)


RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_CONCURRENCY_CONFORMANCE_ACTION,
    summary="validate runnable concurrency conformance across the integrated live workflow",
    backend=RUNNABLE_CONCURRENCY_CONFORMANCE_BACKEND,
    handler=action_validate_concurrency_conformance,
    guarantee_owner=(
        "integrated async/task/executor/actor conformance over the live runtime "
        "architecture workflow"
    ),
)
RUNNABLE_CONCURRENCY_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_CONCURRENCY_ACTION,
    summary="validate runnable concurrency execution end to end from the package root",
    backend=RUNNABLE_CONCURRENCY_E2E_BACKEND,
    handler=action_validate_runnable_concurrency,
    guarantee_owner=(
        "packaged compile, concurrency probe execution, smoke, and replay from the "
        "staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION",
    "RUNNABLE_CONCURRENCY_CONFORMANCE_BACKEND",
    "RUNNABLE_CONCURRENCY_CONFORMANCE_PY",
    "RUNNABLE_CONCURRENCY_CONFORMANCE_SCRIPT",
    "RUNNABLE_CONCURRENCY_E2E_ACTION",
    "RUNNABLE_CONCURRENCY_E2E_BACKEND",
    "RUNNABLE_CONCURRENCY_E2E_PY",
    "RUNNABLE_CONCURRENCY_E2E_SCRIPT",
    "RUNNABLE_CONCURRENCY_ROUTE",
    "VALIDATE_CONCURRENCY_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_CONCURRENCY_ACTION",
    "action_validate_concurrency_conformance",
    "action_validate_runnable_concurrency",
]
