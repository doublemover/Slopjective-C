from __future__ import annotations

import importlib
from pathlib import Path
from types import ModuleType

from scripts.objc3c_workflow.action_catalog_native_package import (
    NATIVE_PACKAGE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_native_package_proof import (
    NATIVE_PACKAGE_PROOF_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_native_package_toolchain import (
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS,
    NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY,
)
from scripts.objc3c_workflow.action_handlers_native_package import (
    NATIVE_PACKAGE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_native_package_proof import (
    NATIVE_PACKAGE_PROOF_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_native_package_toolchain import (
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.actions.package import category_owner_contracts

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

CATALOG_OWNER_MODULES = (
    "action_catalog_native_package_toolchain",
    "action_catalog_native_package_proof",
)
HANDLER_OWNER_MODULES = (
    "action_handlers_native_package_toolchain",
    "action_handlers_native_package_proof",
)


def catalog_owner_modules() -> tuple[str, ...]:
    return CATALOG_OWNER_MODULES


def handler_owner_modules() -> tuple[str, ...]:
    return HANDLER_OWNER_MODULES


def import_workflow_owner_module(module_name: str) -> ModuleType:
    return importlib.import_module(f"scripts.objc3c_workflow.{module_name}")


def workflow_source_text(relative_path: str) -> str:
    return (WORKFLOW_ROOT / relative_path).read_text(encoding="utf-8")


def native_package_catalog_facade_text() -> str:
    return workflow_source_text("action_catalog_native_package.py")


def native_package_handler_facade_text() -> str:
    return workflow_source_text("action_handlers_native_package.py")


def native_package_owner_handler_texts() -> tuple[str, str]:
    return (
        workflow_source_text("action_handlers_native_package_toolchain.py"),
        workflow_source_text("action_handlers_native_package_proof.py"),
    )
