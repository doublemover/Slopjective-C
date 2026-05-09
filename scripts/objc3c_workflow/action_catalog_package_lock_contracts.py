"""Package lock contracts and public workflow actions."""

from __future__ import annotations

from .action_catalog_package_public_workflows import (
    PackagePublicWorkflowAction,
    PackageSchemaContract,
)

PACKAGE_LOCK_SCHEMA = PackageSchemaContract(
    contract_key="package_lock",
    schema_path="schemas/objc3c-package-lock-v1.schema.json",
    schema_id="https://objc3c.dev/schemas/objc3c-package-lock-v1.schema.json",
    document_contract_id="objc3c.package_ecosystem.lockfile.v1",
)

PACKAGE_LOCK_ARTIFACT_PATH = (
    "tmp/artifacts/package-ecosystem/locks/objc3c-package-lock.json"
)

PACKAGE_LOCK_SOURCE_PATHS = (
    "stdlib/workspace.json",
    "stdlib/package_surface.json",
    "stdlib/advanced_helper_package_surface.json",
    "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
    "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json",
)

PACKAGE_LOCK_PUBLIC_ACTIONS = (
    PackagePublicWorkflowAction(
        action="build-package-lock",
        summary=(
            "materialize the checked-in package lock contract from stdlib and "
            "showcase package surfaces"
        ),
        script_path="scripts/build_objc3c_package_lock.py",
        validation_tier="repo",
        guarantee_owner=(
            "package locks stay deterministic, provenance-bearing, and derived "
            "from checked-in local package surfaces"
        ),
        schema_contracts=(PACKAGE_LOCK_SCHEMA,),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(PACKAGE_LOCK_ARTIFACT_PATH,),
    ),
    PackagePublicWorkflowAction(
        action="validate-package-authoring",
        summary=(
            "enforce local package authoring against the package lock contract "
            "and public workflow bridge"
        ),
        script_path="scripts/check_objc3c_package_authoring_workflow.py",
        validation_tier="repo",
        guarantee_owner=(
            "package authoring stays replayable through checked-in package "
            "surfaces and public workflow commands"
        ),
        schema_contracts=(PACKAGE_LOCK_SCHEMA,),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(PACKAGE_LOCK_ARTIFACT_PATH,),
    ),
)


__all__ = [
    "PACKAGE_LOCK_ARTIFACT_PATH",
    "PACKAGE_LOCK_PUBLIC_ACTIONS",
    "PACKAGE_LOCK_SCHEMA",
    "PACKAGE_LOCK_SOURCE_PATHS",
]
