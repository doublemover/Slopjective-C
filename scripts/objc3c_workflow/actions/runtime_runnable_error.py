"""Runnable error workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_ERROR_ROUTE = "error"
VALIDATE_ERROR_CONFORMANCE_ACTION = f"validate-{RUNNABLE_ERROR_ROUTE}-conformance"
VALIDATE_RUNNABLE_ERROR_ACTION = f"validate-runnable-{RUNNABLE_ERROR_ROUTE}"

RUNNABLE_ERROR_CONFORMANCE_SCRIPT = "scripts/check_objc3c_runnable_error_conformance.py"
RUNNABLE_ERROR_E2E_SCRIPT = "scripts/check_objc3c_runnable_error_end_to_end.py"
RUNNABLE_ERROR_CONFORMANCE_BACKEND = f"python:{RUNNABLE_ERROR_CONFORMANCE_SCRIPT}"
RUNNABLE_ERROR_E2E_BACKEND = f"python:{RUNNABLE_ERROR_E2E_SCRIPT}"
RUNNABLE_ERROR_CONFORMANCE_PY = ROOT / RUNNABLE_ERROR_CONFORMANCE_SCRIPT
RUNNABLE_ERROR_E2E_PY = ROOT / RUNNABLE_ERROR_E2E_SCRIPT


def action_validate_error_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_CONFORMANCE_PY)


def action_validate_runnable_error(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_E2E_PY)


RUNNABLE_ERROR_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_ERROR_CONFORMANCE_ACTION,
    summary="validate runnable error conformance across the integrated live workflow",
    backend=RUNNABLE_ERROR_CONFORMANCE_BACKEND,
    handler=action_validate_error_conformance,
    guarantee_owner="integrated error conformance over the live runtime architecture workflow",
)
RUNNABLE_ERROR_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_ERROR_ACTION,
    summary="validate runnable error execution end to end from the package root",
    backend=RUNNABLE_ERROR_E2E_BACKEND,
    handler=action_validate_runnable_error,
    guarantee_owner=(
        "packaged compile, error probe execution, smoke, and replay from the staged "
        "runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_ERROR_CONFORMANCE_ACTION",
    "RUNNABLE_ERROR_CONFORMANCE_BACKEND",
    "RUNNABLE_ERROR_CONFORMANCE_PY",
    "RUNNABLE_ERROR_CONFORMANCE_SCRIPT",
    "RUNNABLE_ERROR_E2E_ACTION",
    "RUNNABLE_ERROR_E2E_BACKEND",
    "RUNNABLE_ERROR_E2E_PY",
    "RUNNABLE_ERROR_E2E_SCRIPT",
    "RUNNABLE_ERROR_ROUTE",
    "VALIDATE_ERROR_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_ERROR_ACTION",
    "action_validate_error_conformance",
    "action_validate_runnable_error",
]
