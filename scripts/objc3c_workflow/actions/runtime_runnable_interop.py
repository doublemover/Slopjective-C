"""Runnable interop workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_INTEROP_ROUTE = "interop"
VALIDATE_INTEROP_CONFORMANCE_ACTION = f"validate-{RUNNABLE_INTEROP_ROUTE}-conformance"
VALIDATE_RUNNABLE_INTEROP_ACTION = f"validate-runnable-{RUNNABLE_INTEROP_ROUTE}"

RUNNABLE_INTEROP_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_interop_conformance.py"
)
RUNNABLE_INTEROP_E2E_SCRIPT = "scripts/check_objc3c_runnable_interop_end_to_end.py"
RUNNABLE_INTEROP_CONFORMANCE_BACKEND = f"python:{RUNNABLE_INTEROP_CONFORMANCE_SCRIPT}"
RUNNABLE_INTEROP_E2E_BACKEND = f"python:{RUNNABLE_INTEROP_E2E_SCRIPT}"
RUNNABLE_INTEROP_CONFORMANCE_PY = ROOT / RUNNABLE_INTEROP_CONFORMANCE_SCRIPT
RUNNABLE_INTEROP_E2E_PY = ROOT / RUNNABLE_INTEROP_E2E_SCRIPT


def action_validate_interop_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_CONFORMANCE_PY)


def action_validate_runnable_interop(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_E2E_PY)


RUNNABLE_INTEROP_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_INTEROP_CONFORMANCE_ACTION,
    summary=(
        "validate runnable mixed-module and interop conformance across the integrated "
        "live workflow"
    ),
    backend=RUNNABLE_INTEROP_CONFORMANCE_BACKEND,
    handler=action_validate_interop_conformance,
    guarantee_owner=(
        "integrated mixed-module runtime packaging and interop conformance over the "
        "live runtime architecture workflow"
    ),
)
RUNNABLE_INTEROP_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_INTEROP_ACTION,
    summary=(
        "validate runnable mixed-module and interop execution end to end from the "
        "package root"
    ),
    backend=RUNNABLE_INTEROP_E2E_BACKEND,
    handler=action_validate_runnable_interop,
    guarantee_owner=(
        "packaged compile, interop probe execution, smoke, and replay from the staged "
        "runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_INTEROP_CONFORMANCE_ACTION",
    "RUNNABLE_INTEROP_CONFORMANCE_BACKEND",
    "RUNNABLE_INTEROP_CONFORMANCE_PY",
    "RUNNABLE_INTEROP_CONFORMANCE_SCRIPT",
    "RUNNABLE_INTEROP_E2E_ACTION",
    "RUNNABLE_INTEROP_E2E_BACKEND",
    "RUNNABLE_INTEROP_E2E_PY",
    "RUNNABLE_INTEROP_E2E_SCRIPT",
    "RUNNABLE_INTEROP_ROUTE",
    "VALIDATE_INTEROP_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_INTEROP_ACTION",
    "action_validate_interop_conformance",
    "action_validate_runnable_interop",
]
