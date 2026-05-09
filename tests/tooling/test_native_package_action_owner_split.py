from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_native_package import (
    NATIVE_PACKAGE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_native_package_proof import (
    NATIVE_PACKAGE_PROOF_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_native_package_toolchain import (
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS,
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


def test_native_package_catalog_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_catalog_native_package.py").read_text(
        encoding="utf-8"
    )

    for module_name in CATALOG_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "ActionSpec(" not in facade_text
    assert "pwsh:scripts/" not in facade_text


def test_native_package_handlers_are_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "action_handlers_native_package.py").read_text(
        encoding="utf-8"
    )

    for module_name in HANDLER_OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from scripts.objc3c_workflow.{module_name} import" in facade_text
    assert "native_build.action_" not in facade_text


def test_native_package_public_order_and_owner_membership() -> None:
    assert tuple(NATIVE_PACKAGE_ACTION_SPECS) == (
        "package-runnable-toolchain",
        "proof-objc3c",
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["package-runnable-toolchain"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain"]
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["proof-objc3c"] is (
        NATIVE_PACKAGE_PROOF_ACTION_SPECS["proof-objc3c"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["package-runnable-toolchain"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS["package-runnable-toolchain"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["proof-objc3c"] is (
        NATIVE_PACKAGE_PROOF_ACTION_HANDLERS["proof-objc3c"]
    )
