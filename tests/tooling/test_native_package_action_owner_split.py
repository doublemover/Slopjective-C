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


def test_native_package_owner_handlers_do_not_route_through_native_build_wrapper() -> None:
    toolchain_text = (
        WORKFLOW_ROOT / "action_handlers_native_package_toolchain.py"
    ).read_text(encoding="utf-8")
    proof_text = (WORKFLOW_ROOT / "action_handlers_native_package_proof.py").read_text(
        encoding="utf-8"
    )

    assert "from scripts.objc3c_workflow.actions import native_build_package" in toolchain_text
    assert "from scripts.objc3c_workflow.actions import native_build_package" in proof_text
    assert "from scripts.objc3c_workflow.actions import native_build\n" not in toolchain_text
    assert "from scripts.objc3c_workflow.actions import native_build\n" not in proof_text
    assert "native_build.action_" not in toolchain_text
    assert "native_build.action_" not in proof_text


def test_runnable_toolchain_package_spec_publishes_platform_owner_contract() -> None:
    assert NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY == {
        "source_owner": "packaging-channels-source",
        "gate_owner": "packaging-channels-gate",
        "blocker_owner": "packaging-channels-blockers",
        "payload_owner_action": "package-runnable-toolchain",
        "unsupported_host_success_allowed": False,
        "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
        "toolchain_archive_claim_requires_owner": True,
        "wrapper_only_action_surface_allowed": False,
    }

    spec = NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain"]
    assert "unsupported hosts cannot succeed" in spec.guarantee_owner
    assert "platform-hardening-build-package-validation" in spec.guarantee_owner
    assert "checked-in package script" in spec.guarantee_owner


def test_package_inventory_facade_publishes_category_owner_contracts() -> None:
    assert category_owner_contracts() == {
        "package": {
            "action_surface_owner": "native-package-toolchain",
            "source_owner": "packaging-channels-source",
            "gate_owner": "packaging-channels-gate",
            "blocker_owner": "packaging-channels-blockers",
            "wrapper_only_action_surface_allowed": False,
        },
        "packaging": {
            "action_surface_owner": "packaging-channels",
            "source_owner": "packaging-channels-source",
            "gate_owner": "packaging-channels-gate",
            "blocker_owner": "packaging-channels-blockers",
            "wrapper_only_action_surface_allowed": False,
        },
    }


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
