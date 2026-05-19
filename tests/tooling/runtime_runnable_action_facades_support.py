from __future__ import annotations

import importlib
from types import ModuleType

from runtime_runnable_action_support import (
    ACTION_ROOT,
    OWNER_EXPORTS,
    OWNER_MODULES,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
)

FACADE_NAMES = ("runtime_runnable_conformance", "runtime_runnable_e2e")


def workflow_action_module(module_name: str) -> ModuleType:
    return importlib.import_module(f"scripts.objc3c_workflow.actions.{module_name}")


def facade_text(facade_name: str) -> str:
    return (ACTION_ROOT / f"{facade_name}.py").read_text(encoding="utf-8")


def facade_requires_owner_module(facade_name: str, module_name: str) -> bool:
    if module_name == "runtime_runnable_bootstrap":
        return facade_name == "runtime_runnable_e2e"
    return True
