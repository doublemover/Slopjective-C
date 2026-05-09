"""Runnable object-model workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_OBJECT_MODEL_ROUTE = "object-model"
VALIDATE_OBJECT_MODEL_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_OBJECT_MODEL_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_OBJECT_MODEL_ACTION = (
    f"validate-runnable-{RUNNABLE_OBJECT_MODEL_ROUTE}"
)

RUNNABLE_OBJECT_MODEL_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_object_model_conformance.py"
)
RUNNABLE_OBJECT_MODEL_E2E_SCRIPT = (
    "scripts/check_objc3c_runnable_object_model_end_to_end.py"
)
RUNNABLE_OBJECT_MODEL_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_OBJECT_MODEL_CONFORMANCE_SCRIPT}"
)
RUNNABLE_OBJECT_MODEL_E2E_BACKEND = f"python:{RUNNABLE_OBJECT_MODEL_E2E_SCRIPT}"
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = ROOT / RUNNABLE_OBJECT_MODEL_CONFORMANCE_SCRIPT
RUNNABLE_OBJECT_MODEL_E2E_PY = ROOT / RUNNABLE_OBJECT_MODEL_E2E_SCRIPT


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_E2E_PY)


RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_OBJECT_MODEL_CONFORMANCE_ACTION,
    summary="validate runnable object-model conformance across the integrated live workflow",
    backend=RUNNABLE_OBJECT_MODEL_CONFORMANCE_BACKEND,
    handler=action_validate_object_model_conformance,
    guarantee_owner=(
        "integrated object-model conformance over the live runtime architecture workflow"
    ),
)
RUNNABLE_OBJECT_MODEL_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_OBJECT_MODEL_ACTION,
    summary="validate runnable object-model execution end to end from the package root",
    backend=RUNNABLE_OBJECT_MODEL_E2E_BACKEND,
    handler=action_validate_runnable_object_model,
    guarantee_owner=(
        "packaged compile, object-model probe execution, smoke, and replay from the "
        "staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION",
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_BACKEND",
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY",
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_SCRIPT",
    "RUNNABLE_OBJECT_MODEL_E2E_ACTION",
    "RUNNABLE_OBJECT_MODEL_E2E_BACKEND",
    "RUNNABLE_OBJECT_MODEL_E2E_PY",
    "RUNNABLE_OBJECT_MODEL_E2E_SCRIPT",
    "RUNNABLE_OBJECT_MODEL_ROUTE",
    "VALIDATE_OBJECT_MODEL_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_OBJECT_MODEL_ACTION",
    "action_validate_object_model_conformance",
    "action_validate_runnable_object_model",
]
