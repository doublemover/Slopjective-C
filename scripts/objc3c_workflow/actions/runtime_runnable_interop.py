"""Runnable interop workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_INTEROP_ROUTE = "interop"
VALIDATE_INTEROP_CONFORMANCE_ACTION = f"validate-{RUNNABLE_INTEROP_ROUTE}-conformance"
VALIDATE_RUNNABLE_INTEROP_ACTION = f"validate-runnable-{RUNNABLE_INTEROP_ROUTE}"
VALIDATE_MODULE_INTEROP_CONTRACTS_ACTION = "validate-module-interop-contracts"
VALIDATE_STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_ACTION = (
    "validate-standalone-textual-interface-payload"
)

RUNNABLE_INTEROP_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_interop_conformance.py"
)
RUNNABLE_INTEROP_E2E_SCRIPT = "scripts/check_objc3c_runnable_interop_end_to_end.py"
MODULE_INTEROP_CONTRACTS_SCRIPT = "scripts/check_objc3c_module_interop_contracts.py"
STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_SCRIPT = (
    "scripts/check_objc3c_standalone_textual_interface_payload.py"
)
RUNNABLE_INTEROP_CONFORMANCE_BACKEND = f"python:{RUNNABLE_INTEROP_CONFORMANCE_SCRIPT}"
RUNNABLE_INTEROP_E2E_BACKEND = f"python:{RUNNABLE_INTEROP_E2E_SCRIPT}"
MODULE_INTEROP_CONTRACTS_BACKEND = f"python:{MODULE_INTEROP_CONTRACTS_SCRIPT}"
STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_BACKEND = (
    f"python:{STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_SCRIPT}"
)
RUNNABLE_INTEROP_CONFORMANCE_PY = ROOT / RUNNABLE_INTEROP_CONFORMANCE_SCRIPT
RUNNABLE_INTEROP_E2E_PY = ROOT / RUNNABLE_INTEROP_E2E_SCRIPT
MODULE_INTEROP_CONTRACTS_PY = ROOT / MODULE_INTEROP_CONTRACTS_SCRIPT
STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_PY = (
    ROOT / STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_SCRIPT
)


def action_validate_interop_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_CONFORMANCE_PY)


def action_validate_runnable_interop(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_E2E_PY)


def action_validate_module_interop_contracts(_: list[str]) -> int:
    return run_python_check(MODULE_INTEROP_CONTRACTS_PY)


def action_validate_standalone_textual_interface_payload(_: list[str]) -> int:
    return run_python_check(STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_PY)


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
MODULE_INTEROP_CONTRACTS_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_MODULE_INTEROP_CONTRACTS_ACTION,
    summary=(
        "validate module import identity, visibility, rebuild, package, and "
        "foreign interop bridge contracts"
    ),
    backend=MODULE_INTEROP_CONTRACTS_BACKEND,
    handler=action_validate_module_interop_contracts,
    guarantee_owner=(
        "checked-in module/interop owner contract for cross-module import "
        "identity and C, ObjC2, Swift, and C++ bridge metadata"
    ),
    validation_tier="targeted",
)
STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_ACTION,
    summary=(
        "validate standalone textual interface payload import, roundtrip, "
        "package lock identity, and fail-closed drift contracts"
    ),
    backend=STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_BACKEND,
    handler=action_validate_standalone_textual_interface_payload,
    guarantee_owner=(
        "checked-in standalone textual interface payload importer and schema "
        "contract for separate-compilation module boundaries"
    ),
    validation_tier="targeted",
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
    "MODULE_INTEROP_CONTRACTS_ACTION",
    "MODULE_INTEROP_CONTRACTS_BACKEND",
    "MODULE_INTEROP_CONTRACTS_PY",
    "MODULE_INTEROP_CONTRACTS_SCRIPT",
    "STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_ACTION",
    "STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_BACKEND",
    "STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_PY",
    "STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_SCRIPT",
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
    "VALIDATE_MODULE_INTEROP_CONTRACTS_ACTION",
    "VALIDATE_RUNNABLE_INTEROP_ACTION",
    "VALIDATE_STANDALONE_TEXTUAL_INTERFACE_PAYLOAD_ACTION",
    "action_validate_module_interop_contracts",
    "action_validate_standalone_textual_interface_payload",
    "action_validate_interop_conformance",
    "action_validate_runnable_interop",
]
