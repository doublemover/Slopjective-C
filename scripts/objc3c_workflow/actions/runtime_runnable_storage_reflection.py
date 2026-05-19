"""Runnable storage/reflection workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check
from objc3c_runtime_acceptance.domains.storage_reflection_owner_contracts import (
    STORAGE_REFLECTION_OWNER_CONTRACT_ID,
    STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
    STRICT_STATUS_OWNER,
)

RUNNABLE_STORAGE_REFLECTION_ROUTE = "storage-reflection"
VALIDATE_STORAGE_REFLECTION_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_STORAGE_REFLECTION_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_STORAGE_REFLECTION_ACTION = (
    f"validate-runnable-{RUNNABLE_STORAGE_REFLECTION_ROUTE}"
)

RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_storage_reflection_conformance.py"
)
RUNNABLE_STORAGE_REFLECTION_E2E_SCRIPT = (
    "scripts/check_objc3c_runnable_storage_reflection_end_to_end.py"
)
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_SCRIPT}"
)
RUNNABLE_STORAGE_REFLECTION_E2E_BACKEND = (
    f"python:{RUNNABLE_STORAGE_REFLECTION_E2E_SCRIPT}"
)
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = (
    ROOT / RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_SCRIPT
)
RUNNABLE_STORAGE_REFLECTION_E2E_PY = ROOT / RUNNABLE_STORAGE_REFLECTION_E2E_SCRIPT
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_GUARANTEE_OWNER = (
    "integrated storage/accessor/reflection conformance over the live runtime "
    "architecture workflow; owner_contract="
    f"{STORAGE_REFLECTION_OWNER_CONTRACT_ID}; status_owner={STRICT_STATUS_OWNER}; "
    f"status_contract={STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID}"
)
RUNNABLE_STORAGE_REFLECTION_E2E_GUARANTEE_OWNER = (
    "packaged compile, storage/reflection probe execution, smoke, and replay "
    "from the staged runnable toolchain bundle; owner_contract="
    f"{STORAGE_REFLECTION_OWNER_CONTRACT_ID}; status_owner={STRICT_STATUS_OWNER}; "
    f"status_contract={STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID}"
)


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_E2E_PY)


RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_STORAGE_REFLECTION_CONFORMANCE_ACTION,
    summary=(
        "validate runnable storage/reflection conformance across the integrated live "
        "workflow"
    ),
    backend=RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_BACKEND,
    handler=action_validate_storage_reflection_conformance,
    guarantee_owner=RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_GUARANTEE_OWNER,
)
RUNNABLE_STORAGE_REFLECTION_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_STORAGE_REFLECTION_ACTION,
    summary=(
        "validate runnable storage/reflection execution end to end from the package "
        "root"
    ),
    backend=RUNNABLE_STORAGE_REFLECTION_E2E_BACKEND,
    handler=action_validate_runnable_storage_reflection,
    guarantee_owner=RUNNABLE_STORAGE_REFLECTION_E2E_GUARANTEE_OWNER,
)


__all__ = [
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_BACKEND",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_GUARANTEE_OWNER",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_SCRIPT",
    "RUNNABLE_STORAGE_REFLECTION_E2E_ACTION",
    "RUNNABLE_STORAGE_REFLECTION_E2E_BACKEND",
    "RUNNABLE_STORAGE_REFLECTION_E2E_GUARANTEE_OWNER",
    "RUNNABLE_STORAGE_REFLECTION_E2E_PY",
    "RUNNABLE_STORAGE_REFLECTION_E2E_SCRIPT",
    "RUNNABLE_STORAGE_REFLECTION_ROUTE",
    "VALIDATE_RUNNABLE_STORAGE_REFLECTION_ACTION",
    "VALIDATE_STORAGE_REFLECTION_CONFORMANCE_ACTION",
    "action_validate_storage_reflection_conformance",
    "action_validate_runnable_storage_reflection",
]
