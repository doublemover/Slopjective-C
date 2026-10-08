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
    "tests/tooling/fixtures/package_ecosystem/package_ecosystem_umbrella_contract.json",
    "tests/tooling/fixtures/package_ecosystem/direct_import_module_syntax_contract.json",
    "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
    "tests/tooling/fixtures/package_ecosystem/negative_package_metadata_contracts.json",
    "tests/tooling/fixtures/package_ecosystem/package_security_hardening_contract.json",
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
            "digest subjects, repo-relative non-overwriting trust paths, "
            "installer/update-key policy hooks, and the reserved production backend"
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
            "production signing before any unsafe trust path can succeed"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_SIGNING_TRUST_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="validate-package-security-hardening",
        summary=(
            "validate package trust policy, extraction path safety, "
            "installer/update-key reservations, and release/registry trust-root "
            "fail-closed contracts"
        ),
        script_path="scripts/check_objc3c_package_security_hardening.py",
        validation_tier="repo",
        guarantee_owner=(
            "package security claims stay bound to repo-relative trust inputs, "
            "non-overwriting signature outputs, pre-mutation extraction plans, "
            "reserved installer/update keys, and reserved release/registry "
            "trust roots"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_LOCK_SCHEMA, PACKAGE_SIGNING_TRUST_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(
            "tmp/reports/package-ecosystem/package-security-hardening-summary.json",
        ),
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
        action="validate-direct-import-module-syntax",
        summary=(
            "validate direct @import token, parser, AST, package provenance, "
            "and fail-closed negative metadata contracts"
        ),
        script_path="scripts/check_objc3c_direct_import_module_syntax.py",
        validation_tier="repo",
        guarantee_owner=(
            "direct @import claims stay source-owned, parser-admitted only as "
            "deterministic module identity records, and locked to package "
            "provenance before resolution"
        ),
        schema_contracts=(PACKAGE_MANIFEST_SCHEMA, PACKAGE_LOCK_SCHEMA),
        source_paths=PACKAGE_LOCK_SOURCE_PATHS,
        generated_paths=(
            "tmp/reports/package-ecosystem/direct-import-module-syntax-summary.json",
        ),
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
