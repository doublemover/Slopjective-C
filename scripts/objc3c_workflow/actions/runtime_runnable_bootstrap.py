"""Runnable bootstrap workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_BOOTSTRAP_ROUTE = "bootstrap"
VALIDATE_RUNNABLE_BOOTSTRAP_ACTION = f"validate-runnable-{RUNNABLE_BOOTSTRAP_ROUTE}"

RUNNABLE_BOOTSTRAP_E2E_SCRIPT = "scripts/check_objc3c_runnable_bootstrap_end_to_end.py"
RUNNABLE_BOOTSTRAP_E2E_BACKEND = f"python:{RUNNABLE_BOOTSTRAP_E2E_SCRIPT}"
RUNNABLE_BOOTSTRAP_E2E_PY = ROOT / RUNNABLE_BOOTSTRAP_E2E_SCRIPT


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BOOTSTRAP_E2E_PY)


RUNNABLE_BOOTSTRAP_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_BOOTSTRAP_ACTION,
    summary="validate the staged runnable toolchain end to end from the package root",
    backend=RUNNABLE_BOOTSTRAP_E2E_BACKEND,
    handler=action_validate_runnable_bootstrap,
    guarantee_owner=(
        "packaged compile, smoke, and replay from the staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_BOOTSTRAP_E2E_ACTION",
    "RUNNABLE_BOOTSTRAP_E2E_BACKEND",
    "RUNNABLE_BOOTSTRAP_E2E_PY",
    "RUNNABLE_BOOTSTRAP_E2E_SCRIPT",
    "RUNNABLE_BOOTSTRAP_ROUTE",
    "VALIDATE_RUNNABLE_BOOTSTRAP_ACTION",
    "action_validate_runnable_bootstrap",
]
