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

PACKAGE_MANIFEST_SCHEMA = PackageSchemaContract(
    contract_key="package_manifest",
    schema_path="schemas/objc3c-package-manifest-v1.schema.json",
    schema_id="https://objc3c.dev/schemas/objc3c-package-manifest-v1.schema.json",
    document_contract_id="objc3c.package_ecosystem.package_manifest.v1",
)

PACKAGE_SIGNING_TRUST_SCHEMA = PackageSchemaContract(
    contract_key="package_signing_trust",
    schema_path="schemas/objc3c-package-signing-trust-v1.schema.json",
    schema_id="https://objc3c.dev/schemas/objc3c-package-signing-trust-v1.schema.json",
    document_contract_id="objc3c.package_ecosystem.signing_trust.v1",
)

PACKAGE_LOCK_ARTIFACT_PATH = (
    "tmp/artifacts/package-ecosystem/locks/objc3c-package-lock.json"
)
PACKAGE_MANIFEST_ARTIFACT_ROOT = (
    "tmp/artifacts/package-ecosystem/manifests"
)

PACKAGE_LOCK_SOURCE_PATHS = (
    "stdlib/workspace.json",
    "stdlib/package_surface.json",
    "stdlib/advanced_helper_package_surface.json",
    "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
    "tests/tooling/fixtures/package_ecosystem/negative_package_metadata_contracts.json",
    "tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json",
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
            "from checked-in local package and module graph surfaces"
        ),
        schema_contracts=(PACKAGE_LOCK_SCHEMA,),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(PACKAGE_LOCK_ARTIFACT_PATH, PACKAGE_MANIFEST_ARTIFACT_ROOT),
    ),
    PackagePublicWorkflowAction(
        action="package-sign",
        summary=(
            "create deterministic local package signature envelopes for "
            "fixtures and fail closed for reserved production signing"
        ),
        script_path="scripts/sign_objc3c_package.py",
        validation_tier="repo",
        guarantee_owner=(
            "package signing stays explicit about trust roots, revocation, "
            "digest subjects, and the reserved production backend"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_SIGNING_TRUST_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="package-verify",
        summary=(
            "verify package manifest digests, signature envelopes, trust roots, "
            "and revocation policy"
        ),
        script_path="scripts/verify_objc3c_package.py",
        validation_tier="repo",
        guarantee_owner=(
            "package verification fails closed for missing signatures, bad "
            "digests, unknown trust roots, revoked subjects, and reserved "
            "production signing"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_SIGNING_TRUST_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="validate-package-manager-model",
        summary=(
            "validate generated package manifests, local dependency resolution, "
            "module graph source truth, language/ABI requirements, and package "
            "trust metadata"
        ),
        script_path="scripts/check_objc3c_package_manager_model.py",
        validation_tier="repo",
        guarantee_owner=(
            "package manager claims stay grounded in generated package "
            "manifests, deterministic local locks, shared module graph metadata, "
            "fail-closed network resolution, and package trust envelopes"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_LOCK_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(PACKAGE_MANIFEST_ARTIFACT_ROOT, PACKAGE_LOCK_ARTIFACT_PATH),
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
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_LOCK_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(PACKAGE_MANIFEST_ARTIFACT_ROOT, PACKAGE_LOCK_ARTIFACT_PATH),
    ),
)


__all__ = [
    "PACKAGE_LOCK_ARTIFACT_PATH",
    "PACKAGE_LOCK_PUBLIC_ACTIONS",
    "PACKAGE_LOCK_SCHEMA",
    "PACKAGE_LOCK_SOURCE_PATHS",
    "PACKAGE_MANIFEST_ARTIFACT_ROOT",
    "PACKAGE_MANIFEST_SCHEMA",
    "PACKAGE_SIGNING_TRUST_SCHEMA",
]
