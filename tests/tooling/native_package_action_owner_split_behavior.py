from __future__ import annotations

from native_package_action_owner_split_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_workflow_modules_import,
)
from native_package_action_owner_split_sources import (
    NATIVE_PACKAGE_ACTION_HANDLERS,
    NATIVE_PACKAGE_ACTION_SPECS,
    NATIVE_PACKAGE_PROOF_ACTION_HANDLERS,
    NATIVE_PACKAGE_PROOF_ACTION_SPECS,
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS,
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS,
    NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY,
    catalog_owner_modules,
    category_owner_contracts,
    handler_owner_modules,
    native_package_catalog_facade_text,
    native_package_handler_facade_text,
    native_package_owner_handler_texts,
)


def assert_native_package_catalog_is_owner_facade() -> None:
    facade_text = native_package_catalog_facade_text()

    assert_workflow_modules_import(catalog_owner_modules())
    for module_name in catalog_owner_modules():
        assert f"from .{module_name} import" in facade_text
    assert_excludes_all(facade_text, ["ActionSpec(", "pwsh:scripts/"])


def assert_native_package_handlers_are_owner_facade() -> None:
    facade_text = native_package_handler_facade_text()

    assert_workflow_modules_import(handler_owner_modules())
    for module_name in handler_owner_modules():
        assert f"from scripts.objc3c_workflow.{module_name} import" in facade_text
    assert_excludes_all(facade_text, ["native_build.action_"])


def assert_native_package_owner_handlers_do_not_route_through_native_build_wrapper() -> None:
    toolchain_text, proof_text = native_package_owner_handler_texts()

    assert_contains_all(
        toolchain_text,
        ["from scripts.objc3c_workflow.actions import native_build_package"],
    )
    assert_contains_all(
        proof_text,
        ["from scripts.objc3c_workflow.actions import native_build_package"],
    )
    assert_excludes_all(
        toolchain_text,
        [
            "from scripts.objc3c_workflow.actions import native_build\n",
            "native_build.action_",
        ],
    )
    assert_excludes_all(
        proof_text,
        [
            "from scripts.objc3c_workflow.actions import native_build\n",
            "native_build.action_",
        ],
    )


def assert_runnable_toolchain_package_spec_publishes_platform_owner_contract() -> None:
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
    assert_contains_all(
        spec.guarantee_owner,
        [
            "unsupported hosts cannot succeed",
            "platform-hardening-build-package-validation",
            "checked-in package script",
        ],
    )


def assert_package_inventory_facade_publishes_category_owner_contracts() -> None:
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


def assert_native_package_public_order_and_owner_membership() -> None:
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
