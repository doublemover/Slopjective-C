"""Package registry publication and offline mirror contracts."""

from __future__ import annotations

from dataclasses import dataclass

from .action_catalog_package_lock_contracts import (
    PACKAGE_LOCK_ARTIFACT_PATH,
    PACKAGE_LOCK_SCHEMA,
    PACKAGE_MANIFEST_ARTIFACT_ROOT,
    PACKAGE_MANIFEST_SCHEMA,
)
from .action_catalog_package_public_workflows import (
    PackagePublicWorkflowAction,
    PackageSchemaContract,
)

PACKAGE_OFFLINE_MIRROR_SCHEMA = PackageSchemaContract(
    contract_key="offline_mirror_index",
    schema_path="schemas/objc3c-package-offline-mirror-index-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-offline-mirror-index-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.offline_mirror.v1",
)

PACKAGE_LOCAL_REGISTRY_INDEX_SCHEMA = PackageSchemaContract(
    contract_key="local_registry_index",
    schema_path="schemas/objc3c-package-local-registry-index-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-local-registry-index-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.local_registry_index.v1",
)

PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA = PackageSchemaContract(
    contract_key="hosted_registry_index",
    schema_path="schemas/objc3c-package-hosted-registry-index-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-hosted-registry-index-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.hosted_registry_index.v1",
)

PACKAGE_HOSTED_REGISTRY_SERVICE_SCHEMA = PackageSchemaContract(
    contract_key="hosted_registry_service",
    schema_path="schemas/objc3c-package-hosted-registry-service-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-hosted-registry-service-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.hosted_registry_service.v1",
)

PACKAGE_NETWORK_RESOLUTION_SCHEMA = PackageSchemaContract(
    contract_key="network_dependency_resolution",
    schema_path="schemas/objc3c-package-network-resolution-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-network-resolution-v1.schema.json"
    ),
    document_contract_id="objc3c.package_ecosystem.network_dependency_resolution.v1",
)

PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA = PackageSchemaContract(
    contract_key="package_release_channel_publication",
    schema_path="schemas/objc3c-package-release-channel-publication-v1.schema.json",
    schema_id=(
        "https://objc3c.dev/schemas/"
        "objc3c-package-release-channel-publication-v1.schema.json"
    ),
    document_contract_id=(
        "objc3c.package_ecosystem.package_release_channel_publication.v1"
    ),
)

PACKAGE_REGISTRY_LOCAL_INDEX_PATH = (
    "tmp/artifacts/package-ecosystem/registry/local-package-index.json"
)
PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH = (
    "tmp/artifacts/package-ecosystem/registry/publication-metadata.json"
)
PACKAGE_OFFLINE_MIRROR_INDEX_PATH = (
    "tmp/artifacts/package-ecosystem/mirrors/offline-mirror-index.json"
)
PACKAGE_OFFLINE_MIRROR_CACHE_ROOT = (
    "tmp/artifacts/package-ecosystem/mirrors/cache"
)
PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH = (
    "tmp/artifacts/package-ecosystem/offline-install/"
    "objc3c-offline-mirror-restore-receipt.json"
)
PACKAGE_HOSTED_REGISTRY_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/"
    "hosted-registry-index.json"
)
PACKAGE_HOSTED_REGISTRY_MIRROR_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/"
    "offline-mirror-index.json"
)
PACKAGE_HOSTED_REGISTRY_NEGATIVE_CASES_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/"
    "negative-registry-cases.json"
)
PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/service/"
    "hosted-registry-service.json"
)
PACKAGE_HOSTED_REGISTRY_SERVICE_NEGATIVE_CASES_PATH = (
    "tests/tooling/fixtures/package_ecosystem/hosted_registry/service/"
    "negative-service-cases.json"
)
PACKAGE_NETWORK_RESOLUTION_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/network_resolution/"
    "network-dependency-resolution.json"
)
PACKAGE_RELEASE_CHANNEL_PUBLICATION_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/network_resolution/"
    "package-release-channel-publication.json"
)
PACKAGE_NETWORK_PUBLICATION_NEGATIVE_CASES_PATH = (
    "tests/tooling/fixtures/package_ecosystem/network_resolution/"
    "negative-network-publication-cases.json"
)
PACKAGE_SECURITY_HARDENING_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/package_security_hardening_contract.json"
)
PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH = (
    "tests/tooling/fixtures/package_ecosystem/package_ecosystem_umbrella_contract.json"
)
PACKAGE_HOSTED_REGISTRY_RESOLUTION_SUMMARY_PATH = (
    "tmp/reports/package-ecosystem/hosted-registry-resolution-summary.json"
)
PACKAGE_NETWORK_PUBLICATION_SUMMARY_PATH = (
    "tmp/reports/package-ecosystem/network-publication-summary.json"
)


@dataclass(frozen=True)
class PackageRegistryPublicationLayer:
    layer_id: str
    source_of_truth: str
    publication_path: str
    claim_boundary: str


PACKAGE_REGISTRY_PUBLICATION_LAYERS = (
    PackageRegistryPublicationLayer(
        layer_id="local-index",
        source_of_truth="lockfile plus package metadata",
        publication_path=PACKAGE_REGISTRY_LOCAL_INDEX_PATH,
        claim_boundary="local generated package discovery",
    ),
    PackageRegistryPublicationLayer(
        layer_id="offline-mirror",
        source_of_truth="lock-derived mirror index",
        publication_path=PACKAGE_OFFLINE_MIRROR_INDEX_PATH,
        claim_boundary="air-gapped local package restore",
    ),
    PackageRegistryPublicationLayer(
        layer_id="publication-metadata",
        source_of_truth="release, update, and package-channel manifests",
        publication_path=PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH,
        claim_boundary="replayable package publication metadata",
    ),
    PackageRegistryPublicationLayer(
        layer_id="hosted-registry-service",
        source_of_truth="source-owned hermetic hosted registry service fixture plus hosted registry index",
        publication_path=PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH,
        claim_boundary="local hosted service contract with fixture auth, trust, revocation, moderation, availability, and no-network fail-closed policy",
    ),
    PackageRegistryPublicationLayer(
        layer_id="network-resolution",
        source_of_truth="source-owned hosted registry fixture, fixture lock, offline mirror, and pinned cache metadata",
        publication_path=PACKAGE_NETWORK_RESOLUTION_FIXTURE_PATH,
        claim_boundary="offline fixture-backed network dependency resolution only; live network fetches and fallback success fail closed",
    ),
    PackageRegistryPublicationLayer(
        layer_id="release-channel-publication",
        source_of_truth="source-owned package release-channel publication contract plus network resolution proof",
        publication_path=PACKAGE_RELEASE_CHANNEL_PUBLICATION_FIXTURE_PATH,
        claim_boundary="package release-channel publication metadata only when channel, lock, trust, provenance, and cache pins remain current",
    ),
    PackageRegistryPublicationLayer(
        layer_id="package-security-hardening",
        source_of_truth="source-owned package security hardening contract plus signing trust schema",
        publication_path=PACKAGE_SECURITY_HARDENING_FIXTURE_PATH,
        claim_boundary="package security claims require pre-mutation extraction plans, repo-relative trust paths, installer/update-key policy hooks, and reserved release/registry trust roots",
    ),
)

PACKAGE_REGISTRY_PUBLIC_ACTIONS = (
    PackagePublicWorkflowAction(
        action="validate-package-mirror",
        summary=(
            "enforce lock-derived offline mirror and local registry metadata "
            "reproducibility"
        ),
        script_path="scripts/check_objc3c_package_registry_mirror_reproducibility.py",
        validation_tier="repo",
        guarantee_owner=(
            "offline mirror and local registry metadata stay lock-derived, "
            "no-network, and fail closed for unsupported hosted-registry claims"
        ),
        schema_contracts=(
            PACKAGE_MANIFEST_SCHEMA,
            PACKAGE_LOCK_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_LOCAL_REGISTRY_INDEX_SCHEMA,
        ),
        source_paths=(
            "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json",
            "tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json",
            PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH,
        ),
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
        action="validate-package-registry-model",
        summary=(
            "validate source-owned hosted-registry fixture metadata, hermetic "
            "service contract, endpoint identity, signatures, digests, "
            "revocations, and offline mirror pins"
        ),
        script_path="scripts/check_objc3c_package_registry_model.py",
        validation_tier="repo",
        guarantee_owner=(
            "hosted registry behavior remains hermetic and offline; live "
            "network fetches fail closed and no live public hosted-registry "
            "support claim is published without production service evidence"
        ),
        schema_contracts=(
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_SERVICE_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
        ),
        source_paths=(
            PACKAGE_HOSTED_REGISTRY_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_NEGATIVE_CASES_PATH,
            PACKAGE_HOSTED_REGISTRY_MIRROR_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_NEGATIVE_CASES_PATH,
            PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH,
        ),
        generated_paths=(PACKAGE_HOSTED_REGISTRY_RESOLUTION_SUMMARY_PATH,),
    ),
    PackagePublicWorkflowAction(
        action="package-registry-resolve",
        summary=(
            "resolve one source-owned hosted-registry fixture record from "
            "checked-in metadata, hermetic service policy, and offline mirror pins"
        ),
        script_path="scripts/check_objc3c_package_registry_model.py",
        validation_tier="repo",
        guarantee_owner=(
            "fixture registry resolution rejects network fetches, missing "
            "metadata, service contract drift, digest/signature drift, "
            "revocations, and ambiguous candidates without fallback registry "
            "success"
        ),
        schema_contracts=(
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_SERVICE_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
        ),
        source_paths=(
            PACKAGE_HOSTED_REGISTRY_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_NEGATIVE_CASES_PATH,
            PACKAGE_HOSTED_REGISTRY_MIRROR_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_NEGATIVE_CASES_PATH,
            PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH,
        ),
        generated_paths=(PACKAGE_HOSTED_REGISTRY_RESOLUTION_SUMMARY_PATH,),
        pass_through_args=True,
    ),
    PackagePublicWorkflowAction(
        action="validate-package-network-publication",
        summary=(
            "validate offline fixture-backed network dependency resolution "
            "and package release-channel publication contracts"
        ),
        script_path="scripts/check_objc3c_package_network_publication.py",
        validation_tier="repo",
        guarantee_owner=(
            "network dependency resolution and package release-channel "
            "publication stay source-owned, lock/trust/provenance bound, "
            "channel aware, cache pinned, and fail closed without fallback "
            "registry success"
        ),
        schema_contracts=(
            PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA,
            PACKAGE_HOSTED_REGISTRY_SERVICE_SCHEMA,
            PACKAGE_OFFLINE_MIRROR_SCHEMA,
            PACKAGE_NETWORK_RESOLUTION_SCHEMA,
            PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA,
        ),
        source_paths=(
            PACKAGE_HOSTED_REGISTRY_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH,
            PACKAGE_HOSTED_REGISTRY_SERVICE_NEGATIVE_CASES_PATH,
            PACKAGE_HOSTED_REGISTRY_MIRROR_FIXTURE_PATH,
            PACKAGE_NETWORK_RESOLUTION_FIXTURE_PATH,
            PACKAGE_RELEASE_CHANNEL_PUBLICATION_FIXTURE_PATH,
            PACKAGE_NETWORK_PUBLICATION_NEGATIVE_CASES_PATH,
            PACKAGE_SECURITY_HARDENING_FIXTURE_PATH,
            PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH,
        ),
        generated_paths=(PACKAGE_NETWORK_PUBLICATION_SUMMARY_PATH,),
    ),
)


__all__ = [
    "PACKAGE_OFFLINE_MIRROR_INDEX_PATH",
    "PACKAGE_OFFLINE_MIRROR_CACHE_ROOT",
    "PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH",
    "PACKAGE_OFFLINE_MIRROR_SCHEMA",
    "PACKAGE_LOCAL_REGISTRY_INDEX_SCHEMA",
    "PACKAGE_HOSTED_REGISTRY_FIXTURE_PATH",
    "PACKAGE_HOSTED_REGISTRY_INDEX_SCHEMA",
    "PACKAGE_HOSTED_REGISTRY_MIRROR_FIXTURE_PATH",
    "PACKAGE_HOSTED_REGISTRY_NEGATIVE_CASES_PATH",
    "PACKAGE_HOSTED_REGISTRY_SERVICE_FIXTURE_PATH",
    "PACKAGE_HOSTED_REGISTRY_SERVICE_NEGATIVE_CASES_PATH",
    "PACKAGE_HOSTED_REGISTRY_SERVICE_SCHEMA",
    "PACKAGE_ECOSYSTEM_UMBRELLA_FIXTURE_PATH",
    "PACKAGE_HOSTED_REGISTRY_RESOLUTION_SUMMARY_PATH",
    "PACKAGE_NETWORK_PUBLICATION_NEGATIVE_CASES_PATH",
    "PACKAGE_NETWORK_PUBLICATION_SUMMARY_PATH",
    "PACKAGE_NETWORK_RESOLUTION_FIXTURE_PATH",
    "PACKAGE_NETWORK_RESOLUTION_SCHEMA",
    "PACKAGE_RELEASE_CHANNEL_PUBLICATION_FIXTURE_PATH",
    "PACKAGE_RELEASE_CHANNEL_PUBLICATION_SCHEMA",
    "PACKAGE_SECURITY_HARDENING_FIXTURE_PATH",
    "PACKAGE_REGISTRY_LOCAL_INDEX_PATH",
    "PACKAGE_REGISTRY_PUBLICATION_LAYERS",
    "PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH",
    "PACKAGE_REGISTRY_PUBLIC_ACTIONS",
    "PackageRegistryPublicationLayer",
]
