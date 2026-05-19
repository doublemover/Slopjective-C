"""Runnable metaprogramming workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_METAPROGRAMMING_ROUTE = "metaprogramming"
VALIDATE_METAPROGRAMMING_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_METAPROGRAMMING_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_METAPROGRAMMING_ACTION = (
    f"validate-runnable-{RUNNABLE_METAPROGRAMMING_ROUTE}"
)

RUNNABLE_METAPROGRAMMING_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_metaprogramming_conformance.py"
)
RUNNABLE_METAPROGRAMMING_E2E_SCRIPT = (
    "scripts/check_objc3c_runnable_metaprogramming_end_to_end.py"
)
RUNNABLE_METAPROGRAMMING_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_METAPROGRAMMING_CONFORMANCE_SCRIPT}"
)
RUNNABLE_METAPROGRAMMING_E2E_BACKEND = (
    f"python:{RUNNABLE_METAPROGRAMMING_E2E_SCRIPT}"
)
RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = (
    ROOT / RUNNABLE_METAPROGRAMMING_CONFORMANCE_SCRIPT
)
RUNNABLE_METAPROGRAMMING_E2E_PY = ROOT / RUNNABLE_METAPROGRAMMING_E2E_SCRIPT


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_E2E_PY)


RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_METAPROGRAMMING_CONFORMANCE_ACTION,
    summary=(
        "validate runnable metaprogramming conformance across the integrated live "
        "workflow"
    ),
    backend=RUNNABLE_METAPROGRAMMING_CONFORMANCE_BACKEND,
    handler=action_validate_metaprogramming_conformance,
    guarantee_owner=(
        "integrated metaprogramming conformance over the live runtime architecture "
        "workflow"
    ),
)
RUNNABLE_METAPROGRAMMING_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_METAPROGRAMMING_ACTION,
    summary="validate runnable metaprogramming execution end to end from the package root",
    backend=RUNNABLE_METAPROGRAMMING_E2E_BACKEND,
    handler=action_validate_runnable_metaprogramming,
    guarantee_owner=(
        "packaged compile, metaprogramming probe execution, smoke, and replay from "
        "the staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION",
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_BACKEND",
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY",
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_SCRIPT",
    "RUNNABLE_METAPROGRAMMING_E2E_ACTION",
    "RUNNABLE_METAPROGRAMMING_E2E_BACKEND",
    "RUNNABLE_METAPROGRAMMING_E2E_PY",
    "RUNNABLE_METAPROGRAMMING_E2E_SCRIPT",
    "RUNNABLE_METAPROGRAMMING_ROUTE",
    "VALIDATE_METAPROGRAMMING_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_METAPROGRAMMING_ACTION",
    "action_validate_metaprogramming_conformance",
    "action_validate_runnable_metaprogramming",
]
