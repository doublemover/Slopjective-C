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
)


__all__ = [
    "PACKAGE_OFFLINE_MIRROR_INDEX_PATH",
    "PACKAGE_OFFLINE_MIRROR_CACHE_ROOT",
    "PACKAGE_OFFLINE_MIRROR_RESTORE_RECEIPT_PATH",
    "PACKAGE_OFFLINE_MIRROR_SCHEMA",
    "PACKAGE_LOCAL_REGISTRY_INDEX_SCHEMA",
    "PACKAGE_REGISTRY_LOCAL_INDEX_PATH",
    "PACKAGE_REGISTRY_PUBLICATION_LAYERS",
    "PACKAGE_REGISTRY_PUBLICATION_METADATA_PATH",
    "PACKAGE_REGISTRY_PUBLIC_ACTIONS",
    "PackageRegistryPublicationLayer",
]
