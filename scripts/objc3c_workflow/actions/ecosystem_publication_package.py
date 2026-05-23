"""Package ecosystem publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_package_contracts import (
    PACKAGE_AUTHORING_WORKFLOW_PY,
    PACKAGE_ECOSYSTEM_INTEGRATION_PY,
    PACKAGE_LOCK_PY,
    PACKAGE_MANAGER_MODEL_PY,
    PACKAGE_MIRROR_REPRODUCIBILITY_PY,
    PACKAGE_REGISTRY_MODEL_PY,
    PACKAGE_SIGN_PY,
    PACKAGE_INSTALL_DISTRIBUTION_PY,
    PACKAGE_OPERATIONS_PY,
    PACKAGE_VERIFY_PY,
    RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY,
)
from .ecosystem_publication_package_runner import run_package_publication_action


def action_build_package_lock(_: list[str]) -> int:
    return run_package_publication_action("build-package-lock")


def action_sign_package(rest: list[str]) -> int:
    return run_package_publication_action("package-sign", rest)


def action_verify_package(rest: list[str]) -> int:
    return run_package_publication_action("package-verify", rest)


def action_validate_package_manager_model(_: list[str]) -> int:
    return run_package_publication_action("validate-package-manager-model")


def action_validate_package_authoring(_: list[str]) -> int:
    return run_package_publication_action("validate-package-authoring")


def action_validate_package_mirror(_: list[str]) -> int:
    return run_package_publication_action("validate-package-mirror")


def action_validate_package_registry_model(_: list[str]) -> int:
    return run_package_publication_action("validate-package-registry-model")


def action_package_registry_resolve(rest: list[str]) -> int:
    return run_package_publication_action("package-registry-resolve", rest)


def action_validate_package_ecosystem(_: list[str]) -> int:
    return run_package_publication_action("validate-package-ecosystem")


def action_validate_package_install_distribution(rest: list[str]) -> int:
    return run_package_publication_action(
        "validate-package-install-distribution",
        rest or ["--from-nothing"],
    )


def action_package_publish(rest: list[str]) -> int:
    return run_package_publication_action("package-publish", ["--operation", "publish", *rest])


def action_package_install(rest: list[str]) -> int:
    return run_package_publication_action("package-install", ["--operation", "install", *rest])


def action_package_update(rest: list[str]) -> int:
    return run_package_publication_action("package-update", ["--operation", "update", *rest])


def action_package_uninstall(rest: list[str]) -> int:
    return run_package_publication_action("package-uninstall", ["--operation", "uninstall", *rest])


def action_package_rollback(rest: list[str]) -> int:
    return run_package_publication_action("package-rollback", ["--operation", "rollback", *rest])


def action_validate_package_operations(rest: list[str]) -> int:
    return run_package_publication_action("validate-package-operations", rest)


def action_validate_runnable_package_ecosystem(_: list[str]) -> int:
    return run_package_publication_action("validate-runnable-package-ecosystem")


__all__ = [
    "PACKAGE_AUTHORING_WORKFLOW_PY",
    "PACKAGE_ECOSYSTEM_INTEGRATION_PY",
    "PACKAGE_LOCK_PY",
    "PACKAGE_MANAGER_MODEL_PY",
    "PACKAGE_MIRROR_REPRODUCIBILITY_PY",
    "PACKAGE_REGISTRY_MODEL_PY",
    "PACKAGE_SIGN_PY",
    "PACKAGE_INSTALL_DISTRIBUTION_PY",
    "PACKAGE_OPERATIONS_PY",
    "PACKAGE_VERIFY_PY",
    "RUNNABLE_PACKAGE_ECOSYSTEM_E2E_PY",
    "action_build_package_lock",
    "action_sign_package",
    "action_verify_package",
    "action_validate_package_manager_model",
    "action_package_registry_resolve",
    "action_package_install",
    "action_package_publish",
    "action_package_rollback",
    "action_package_uninstall",
    "action_package_update",
    "action_validate_package_authoring",
    "action_validate_package_ecosystem",
    "action_validate_package_install_distribution",
    "action_validate_package_mirror",
    "action_validate_package_operations",
    "action_validate_package_registry_model",
    "action_validate_runnable_package_ecosystem",
]
