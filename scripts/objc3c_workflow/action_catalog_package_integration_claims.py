"""Package integration claims and public workflow actions."""

from __future__ import annotations

from .action_catalog_package_install_receipts import PACKAGE_INSTALL_RECEIPT_SCHEMA
from .action_catalog_package_lock_contracts import (
    PACKAGE_LOCK_ARTIFACT_PATH,
    PACKAGE_LOCK_SCHEMA,
    PACKAGE_MANIFEST_ARTIFACT_ROOT,
    PACKAGE_MANIFEST_SCHEMA,
)
from .action_catalog_package_public_workflows import PackagePublicWorkflowAction
from .action_catalog_package_public_workflows import PackageSchemaContract
from .action_catalog_package_registry_publication import (
    PACKAGE_OFFLINE_MIRROR_CACHE_ROOT,
    PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
    PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH,
    PACKAGE_OFFLINE_MIRROR_SCHEMA,
    PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
    PACKAGE_NETWORK_RESOLUTION_SCHEMA,
    PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
    PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
    PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
)

PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS = (
    "tests/tooling/fixtures/package_ecosystem/artifact_contract.json",
    "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json",
    "tests/tooling/fixtures/package_ecosystem/package_ecosystem_umbrella_contract.json",
    "tests/tooling/fixtures/package_ecosystem/direct_import_module_syntax_contract.json",
    "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
    "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json",
    "tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json",
    "tests/tooling/fixtures/package_ecosystem/install_distribution_credibility_contract.json",
    "tests/tooling/fixtures/package_ecosystem/from_nothing_install_proof_contract.json",
    "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json",
    "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json",
    "tests/tooling/fixtures/package_ecosystem/package_security_hardening_contract.json",
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/hosted-registry-index.json",
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/offline-mirror-index.json",
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/negative-registry-cases.json",
    "tests/tooling/fixtures/package_ecosystem/network_resolution/network-dependency-resolution.json",
    "tests/tooling/fixtures/package_ecosystem/network_resolution/package-release-channel-publication.json",
    "tests/tooling/fixtures/package_ecosystem/network_resolution/negative-network-publication-cases.json",
)

PACKAGE_OPERATION_RECEIPT_SCHEMA = PackageSchemaContract(
    contract_key="package_operation_receipt",
    schema_path="schemas/objc3c-package-operation-receipt-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-operation-receipt-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.operation_receipt.v1",
)

PACKAGE_OPERATION_RECEIPT_ROOT = "tmp/artifacts/package-ecosystem/operations"

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
            PACKAGE_MANIFEST_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_INSTALL_RECEIPT_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_NETWORK_RESOLUTION_SCHEMA,
            PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
        ),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(
            PACKAGE_LOCK_ARTIFACT_PATH,
            PACKAGE_MANIFEST_ARTIFACT_ROOT,
            PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
            PACKAGE_OFFLINE_MIRROR_CACHE_ROOT,
            PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH,
            PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
            PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
        ),
    ),
    PackagePublicWorkflowAction(
        action="validate-package-install-distribution",
        summary=(
            "enforce from-nothing clean-root package install credibility "
            "across generated manifests, locks, mirrors, registry metadata, "
            "restore receipts, and install receipts"
        ),
        script_path="scripts/check_objc3c_package_install_distribution_credibility.py",
        validation_tier="repo",
        guarantee_owner=(
            "package install distribution claims stay grounded in from-nothing "
            "clean local install evidence; the public action defaults to "
            "--from-nothing when no pass-through args are supplied"
        ),
        schema_contracts=(
            PACKAGE_LOCK_SCHEMA,
            PACKAGE_MANIFEST_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_INSTALL_RECEIPT_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_NETWORK_RESOLUTION_SCHEMA,
            PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
        ),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(
            PACKAGE_LOCK_ARTIFACT_PATH,
            PACKAGE_MANIFEST_ARTIFACT_ROOT,
            PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
            PACKAGE_OFFLINE_MIRROR_CACHE_ROOT,
            PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH,
            PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
            PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
            "tmp/artifacts/package-ecosystem/install-validation",
            "tmp/artifacts/package-ecosystem/install-validation/local-package-artifacts",
            "tmp/artifacts/package-ecosystem/install-validation/objc3c-install-proof-manifest.json",
        ),
        pass_through_args=True,
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
            PACKAGE_MANIFEST_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_INSTALL_RECEIPT_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_NETWORK_RESOLUTION_SCHEMA,
            PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
        ),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(
            PACKAGE_LOCK_ARTIFACT_PATH,
            PACKAGE_MANIFEST_ARTIFACT_ROOT,
            PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
            PACKAGE_OFFLINE_MIRROR_CACHE_ROOT,
            PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH,
            PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
            PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
        ),
    ),
    PackagePublicWorkflowAction(
        action="package-publish",
        summary="emit a deterministic signed local publish plan and receipt",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "package publish stays local/offline/deterministic and fails "
            "closed for live network publication"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="package-install",
        summary="emit a deterministic signed local install plan and receipt",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "package install verifies trust, registry metadata, lock identity, "
            "offline mirror pins, extraction path safety, language/ABI "
            "compatibility, and dependencies"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="package-update",
        summary="emit a deterministic compatible update plan and receipt",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "package update preserves rollback data and accepts only signed, "
            "locked, cache-pinned compatible metadata with extraction safety"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="package-uninstall",
        summary="emit a deterministic owned-root uninstall plan and receipt",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "package uninstall proves installed-file ownership and refuses "
            "removal outside package-owned roots"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="package-rollback",
        summary="emit a deterministic rollback plan and receipt",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "package rollback restores only signed cache-pinned previous "
            "package state with owned-root proof"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="validate-package-operations",
        summary="validate all deterministic package operation plans and receipts",
        script_path="scripts/check_objc3c_package_operations.py",
        validation_tier="repo",
        guarantee_owner=(
            "publish, install, update, uninstall, and rollback receipts stay "
            "deterministic, trust-bound, cache-pinned, extraction-safe, and "
            "fail-closed"
        ),
        schema_contracts=(PACKAGE_OPERATION_RECEIPT_SCHEMA,),
        source_paths=PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS,
        generated_paths=(PACKAGE_OPERATION_RECEIPT_ROOT,),
        pass_through_args=True,
    ),
)


__all__ = [
    "PACKAGE_INTEGRATION_CLAIM_SOURCE_PATHS",
    "PACKAGE_INTEGRATION_PUBLIC_ACTIONS",
    "PACKAGE_OPERATION_RECEIPT_ROOT",
    "PACKAGE_OPERATION_RECEIPT_SCHEMA",
]
