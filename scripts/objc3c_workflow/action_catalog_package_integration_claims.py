"""Package integration claims and public workflow actions."""

from __future__ import annotations

from .action_catalog_package_install_receipts import PACKAGE_INSTALL_RECEIPT_SCHEMA
from .action_catalog_package_lock_contracts import (
    PACKAGE_LOCK_ARTIFACT_PATH,
    PACKAGE_LOCK_SCHEMA,
)
from .action_catalog_package_public_workflows import PackagePublicWorkflowAction
from .action_catalog_package_registry_publication import (
    PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
    PACKAGE_OFFLINE_MIRROR_SCHEMA,
    PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
    PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
)

PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS = (
    "tests/tooling/fixtures/package_ecosystem/artifact_contract.json",
    "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json",
    "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json",
    "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json",
)

PACKAGE_INTEGRATION_PUBLIC_ACTIONS = (
    PackagePublicWorkflowAction(
        action="validate-package-ecosystem",
        summary=(
            "enforce integrated package ecosystem claims across locks, offline "
            "mirrors, stdlib programs, and canonical application workspaces"
        ),
        script_path="scripts/check_objc3c_package_ecosystem_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "package ecosystem claims stay grounded in deterministic locks, "
            "offline mirrors, stdlib programs, and canonical application "
            "workspaces"
        ),
        schema_contracts=(
            PACKAGE_LOCK_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_INSTALL_RECEIPT_SCHEMA,
        ),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(
            PACKAGE_LOCK_ARTIFACT_PATH,
            PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
            PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
            PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
        ),
    ),
    PackagePublicWorkflowAction(
        action="validate-runnable-package-ecosystem",
        summary=(
            "enforce package authoring, install receipt, and offline mirror "
            "workflows from the staged runnable toolchain bundle"
        ),
        script_path="scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "packaged dependency, install receipt, and offline mirror workflows "
            "stay reproducible from the staged runnable toolchain bundle"
        ),
        schema_contracts=(
            PACKAGE_LOCK_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_INSTALL_RECEIPT_SCHEMA,
        ),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(
            PACKAGE_LOCK_ARTIFACT_PATH,
            PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
            PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
            PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
        ),
    ),
)


__all__ = [
    "PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS",
    "PACKAGE_INTEGRATION_PUBLIC_ACTIONS",
]
