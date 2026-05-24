"""Package channel data model and payload builders."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
import hashlib
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .paths import (
    ARTIFACT_ROOT,
    PLATFORM_SUPPORT_MATRIX_ARTIFACT,
    PLATFORM_SUPPORT_MATRIX_SUMMARY,
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    ROOT,
)


IMPLEMENTED_CHANNELS = ["portable-archive", "local-installer", "offline-bundle"]
SANITIZER_VARIANTS = ("release", "address", "undefined")
MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"
INSTALL_RECEIPT_CONTRACT_ID = "objc3c.packaging.channels.install-receipt.v1"
INSTALL_RECEIPT_SCHEMA = "schemas/objc3c-package-install-receipt-v1.schema.json"
INSTALL_RECEIPT_PATH = "objc3c-install-receipt.json"
INSTALL_COMMAND = "npm run objc3c -- build-package-channels"
INSTALL_BOOTSTRAP_ENTRYPOINT = "Bootstrap-objc3cEnvironment.ps1"
REQUIRED_RECEIPT_FIELDS = [
    "contract_id",
    "install_root",
    "install_home",
    "channel_id",
    "bootstrap_entrypoint",
    "package_bridge",
    "install_command",
    "payload_manifest",
    "payload_manifest_sha256",
    "payload_required_entries",
    "installed_at_utc",
]
SANITIZER_REQUIRED_RECEIPT_FIELDS = [*REQUIRED_RECEIPT_FIELDS, "sanitizer_package_variant"]
REQUIRED_PAYLOAD_ENTRIES = [
    MANIFEST_RELATIVE_PATH,
    "artifacts/bin/objc3c-native.exe",
    "artifacts/lib/objc3_runtime.lib",
    "stdlib/workspace.json",
    "stdlib/modules/objc3.core/module.json",
    "docs/runbooks/objc3c_packaging_channels.md",
]
SANITIZER_PAYLOAD_ENTRIES = {
    "address": ["share/objc3c/sanitizer/asan-metadata.json"],
    "undefined": ["share/objc3c/sanitizer/ubsan-metadata.json"],
}
ARCHIVE_DIGEST_FIELDS = {
    "portable_archive": "portable-archive",
    "installer_archive": "local-installer",
    "offline_archive": "offline-bundle",
}


@dataclass(frozen=True)
class PackageChannelSurfaceInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]
    platform_support_matrix: dict[str, Any]
    interop_loader_metadata: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelPaths:
    run_id: str
    sanitizer_variant: str
    package_id: str
    package_channel_id: str
    package_root: Path
    build_root: Path
    portable_archive: Path
    installer_image_root: Path
    installer_archive: Path
    offline_bundle_root: Path
    offline_archive: Path
    manifest_path: Path


def sanitizer_variant_metadata(sanitizer_variant: str) -> dict[str, Any]:
    if sanitizer_variant == "release":
        return {
            "sanitizer_variant": "release",
            "package_id": "org.objc3c.runtime:objc3c-runtime-release",
            "package_channel_id": "windows-x64-release",
            "archive_suffix": "windows-x64",
            "runtime_variant": "release",
            "support_truth": False,
            "native_execution_claimed": False,
        }
    if sanitizer_variant == "address":
        return {
            "sanitizer_variant": "address",
            "package_id": "org.objc3c.runtime:objc3c-runtime-asan",
            "package_variant_row_id": "objc3c.package.sanitizer.asan.reserved",
            "package_channel_id": "windows-x64-sanitizer-asan",
            "archive_suffix": "windows-x64-asan",
            "runtime_variant": "sanitizer=address",
            "install_selector": "sanitizer=address",
            "metadata_manifest_path": "share/objc3c/sanitizer/asan-metadata.json",
            "runtime_library_ids": ["objc3-runtime", "clang_rt.asan"],
            "support_truth": False,
            "native_execution_claimed": False,
        }
    if sanitizer_variant == "undefined":
        return {
            "sanitizer_variant": "undefined",
            "package_id": "org.objc3c.runtime:objc3c-runtime-ubsan",
            "package_variant_row_id": "objc3c.package.sanitizer.ubsan.reserved",
            "package_channel_id": "windows-x64-sanitizer-ubsan",
            "archive_suffix": "windows-x64-ubsan",
            "runtime_variant": "sanitizer=undefined",
            "install_selector": "sanitizer=undefined",
            "metadata_manifest_path": "share/objc3c/sanitizer/ubsan-metadata.json",
            "runtime_library_ids": ["objc3-runtime", "clang_rt.ubsan"],
            "trap_or_recover_mode": "trap",
            "support_truth": False,
            "native_execution_claimed": False,
        }
    raise ValueError(f"unsupported sanitizer variant: {sanitizer_variant}")


def package_channel_paths(run_id: str, sanitizer_variant: str = "release") -> PackageChannelPaths:
    metadata = sanitizer_variant_metadata(sanitizer_variant)
    package_root_name = "runnable" if sanitizer_variant == "release" else f"runnable-{sanitizer_variant}"
    package_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels" / run_id / package_root_name
    build_root = ARTIFACT_ROOT / run_id / str(metadata["archive_suffix"])
    archive_suffix = str(metadata["archive_suffix"])
    portable_archive = build_root / "portable" / f"objc3c-{archive_suffix}-portable.zip"
    installer_image_root = build_root / "installer" / "image"
    installer_archive = build_root / "installer" / f"objc3c-{archive_suffix}-installer.zip"
    offline_bundle_root = build_root / "offline" / "bundle"
    offline_archive = build_root / "offline" / f"objc3c-{archive_suffix}-offline-bundle.zip"
    manifest_path = build_root / "objc3c-package-channels-manifest.json"
    return PackageChannelPaths(
        run_id=run_id,
        sanitizer_variant=sanitizer_variant,
        package_id=str(metadata["package_id"]),
        package_channel_id=str(metadata["package_channel_id"]),
        package_root=package_root,
        build_root=build_root,
        portable_archive=portable_archive,
        installer_image_root=installer_image_root,
        installer_archive=installer_archive,
        offline_bundle_root=offline_bundle_root,
        offline_archive=offline_archive,
        manifest_path=manifest_path,
    )


def package_channels_manifest_payload(
    *,
    inputs: PackageChannelInputs,
    paths: PackageChannelPaths,
    installer_signature: dict[str, Any],
    archive_digests: dict[str, Any] | None = None,
    payload_contract: dict[str, Any] | None = None,
    receipt_contracts: dict[str, Any] | None = None,
) -> dict[str, Any]:
    resolved_archive_digests = (
        archive_digest_payloads(paths) if archive_digests is None else archive_digests
    )
    resolved_payload_contract = (
        package_payload_contract(paths) if payload_contract is None else payload_contract
    )
    resolved_receipt_contracts = (
        receipt_contract_payloads(paths.sanitizer_variant)
        if receipt_contracts is None
        else receipt_contracts
    )
    return {
        "contract_id": "objc3c.packaging.channels.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": inputs.supported_platforms["default_platform_id"],
        "package_id": paths.package_id,
        "package_channel_id": paths.package_channel_id,
        "sanitizer_variant": paths.sanitizer_variant,
        "package_root": repo_rel(paths.package_root),
        "portable_archive": repo_rel(paths.portable_archive),
        "installer_archive": repo_rel(paths.installer_archive),
        "offline_archive": repo_rel(paths.offline_archive),
        "installer_image_root": repo_rel(paths.installer_image_root),
        "offline_bundle_root": repo_rel(paths.offline_bundle_root),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX_ARTIFACT),
        "platform_support_summary": repo_rel(PLATFORM_SUPPORT_MATRIX_SUMMARY),
        "supported_platform_ids": inputs.platform_support_matrix["claim_boundary"]["supported_platform_ids"],
        "support_tiers": inputs.platform_support_matrix["tiers"],
        "implemented_channels": IMPLEMENTED_CHANNELS,
        "interop_loader_metadata": inputs.interop_loader_metadata,
        "installer_signature": installer_signature,
        "archive_digests": resolved_archive_digests,
        "payload_contract": resolved_payload_contract,
        "receipt_contracts": resolved_receipt_contracts,
        "support_truth": False,
        "native_execution_claimed": False,
        "release_foundation_artifacts": {
            "manifest": repo_rel(RELEASE_FOUNDATION_MANIFEST),
            "sbom": repo_rel(RELEASE_FOUNDATION_SBOM),
            "attestation": repo_rel(RELEASE_FOUNDATION_ATTESTATION),
        },
    }


def package_channels_report_payload(
    *,
    inputs: PackageChannelInputs,
    paths: PackageChannelPaths,
    manifest_payload: dict[str, Any],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.packaging.channels.summary.report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": inputs.supported_platforms["default_platform_id"],
        "package_id": paths.package_id,
        "package_channel_id": paths.package_channel_id,
        "sanitizer_variant": paths.sanitizer_variant,
        "package_root": repo_rel(paths.package_root),
        "manifest_path": repo_rel(paths.manifest_path),
        "portable_archive": repo_rel(paths.portable_archive),
        "installer_image_root": repo_rel(paths.installer_image_root),
        "installer_archive": repo_rel(paths.installer_archive),
        "offline_bundle_root": repo_rel(paths.offline_bundle_root),
        "offline_archive": repo_rel(paths.offline_archive),
        "platform_support_matrix": repo_rel(PLATFORM_SUPPORT_MATRIX_ARTIFACT),
        "supported_platform_ids": inputs.platform_support_matrix["claim_boundary"]["supported_platform_ids"],
        "support_tiers": inputs.platform_support_matrix["tiers"],
        "implemented_channels": manifest_payload["implemented_channels"],
        "interop_loader_metadata": manifest_payload["interop_loader_metadata"],
        "installer_signature": manifest_payload["installer_signature"],
        "archive_digests": manifest_payload["archive_digests"],
        "payload_contract": manifest_payload["payload_contract"],
        "receipt_contracts": manifest_payload["receipt_contracts"],
        "support_truth": manifest_payload["support_truth"],
        "native_execution_claimed": manifest_payload["native_execution_claimed"],
    }


def required_payload_entries(sanitizer_variant: str = "release") -> list[str]:
    return [
        *REQUIRED_PAYLOAD_ENTRIES,
        *SANITIZER_PAYLOAD_ENTRIES.get(sanitizer_variant, []),
    ]


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def installer_signature_payload(installer_archive: Path) -> dict[str, Any]:
    return {
        "signature_format": "objc3c-local-sha256-v1",
        "signing_key_id": "objc3c-release-operations-local-installer-key-v1",
        "subject": "local-installer",
        "artifact": repo_rel(installer_archive),
        "sha256": sha256_file(installer_archive),
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def archive_digest_record(*, artifact_role: str, artifact_path: Path) -> dict[str, str]:
    return {
        "digest_format": "sha256",
        "artifact_role": artifact_role,
        "artifact": repo_rel(artifact_path),
        "sha256": sha256_file(artifact_path),
        "verification_command": "npm run objc3c -- validate-packaging-channels-end-to-end",
        "trust_scope": "checked-in-artifact-digest",
    }


def archive_digest_payloads(paths: PackageChannelPaths) -> dict[str, dict[str, str]]:
    artifact_paths = {
        "portable_archive": paths.portable_archive,
        "installer_archive": paths.installer_archive,
        "offline_archive": paths.offline_archive,
    }
    return {
        field_name: archive_digest_record(
            artifact_role=artifact_role,
            artifact_path=artifact_paths[field_name],
        )
        for field_name, artifact_role in ARCHIVE_DIGEST_FIELDS.items()
    }


def package_payload_contract(paths: PackageChannelPaths) -> dict[str, Any]:
    manifest_path = paths.package_root / MANIFEST_RELATIVE_PATH
    entry_digests = {
        relative_path: package_payload_entry_digest(
            paths=paths,
            relative_path=relative_path,
        )
        for relative_path in required_payload_entries(paths.sanitizer_variant)
    }
    return {
        "contract_id": "objc3c.packaging.channels.payload-contract.v1",
        "source": "canonical-runnable-toolchain-package",
        "manifest_relative_path": MANIFEST_RELATIVE_PATH,
        "manifest_artifact": repo_rel(manifest_path),
        "manifest_sha256": entry_digests[MANIFEST_RELATIVE_PATH]["sha256"],
        "required_entries": required_payload_entries(paths.sanitizer_variant),
        "entry_digests": entry_digests,
        "clean_room_source_policy": "fresh-owned-tmp-root-only",
    }


def package_payload_entry_digest(
    *,
    paths: PackageChannelPaths,
    relative_path: str,
) -> dict[str, str]:
    artifact_path = paths.package_root / relative_path
    if not artifact_path.is_file():
        raise RuntimeError(f"package-channels payload missing required entry {relative_path}")
    return {
        "digest_format": "sha256",
        "artifact": relative_path,
        "sha256": sha256_file(artifact_path),
    }


def receipt_contract_payloads(sanitizer_variant: str = "release") -> dict[str, dict[str, Any]]:
    return {
        "install_receipt": receipt_contract_payload(
            channel_id="local-installer",
            emitted_by="Install-objc3c.ps1",
            network_policy="local-filesystem-only",
            sanitizer_variant=sanitizer_variant,
        ),
        "offline_install_receipt": receipt_contract_payload(
            channel_id="offline-bundle",
            emitted_by="OfflineBootstrap-objc3c.ps1",
            network_policy="no-network",
            delegates_to="local-installer",
            sanitizer_variant=sanitizer_variant,
        ),
    }


def receipt_contract_payload(
    *,
    channel_id: str,
    emitted_by: str,
    network_policy: str,
    sanitizer_variant: str = "release",
    delegates_to: str | None = None,
) -> dict[str, Any]:
    metadata = sanitizer_variant_metadata(sanitizer_variant)
    required_fields = (
        SANITIZER_REQUIRED_RECEIPT_FIELDS
        if sanitizer_variant != "release"
        else REQUIRED_RECEIPT_FIELDS
    )
    payload: dict[str, Any] = {
        "contract_id": INSTALL_RECEIPT_CONTRACT_ID,
        "schema": INSTALL_RECEIPT_SCHEMA,
        "receipt_path": INSTALL_RECEIPT_PATH,
        "channel_id": channel_id,
        "emitted_by": emitted_by,
        "bootstrap_entrypoint": INSTALL_BOOTSTRAP_ENTRYPOINT,
        "package_bridge": "objc3c",
        "install_command": INSTALL_COMMAND,
        "payload_manifest": MANIFEST_RELATIVE_PATH,
        "payload_required_entries": required_payload_entries(sanitizer_variant),
        "required_fields": required_fields,
        "network_policy": network_policy,
        "rollback_required": True,
        "package_id": metadata["package_id"],
        "package_channel_id": metadata["package_channel_id"],
        "sanitizer_variant": sanitizer_variant,
        "support_truth": False,
        "native_execution_claimed": False,
    }
    if sanitizer_variant != "release":
        payload["sanitizer_install_selector"] = metadata["install_selector"]
    if delegates_to is not None:
        payload["delegates_to"] = delegates_to
    return payload
