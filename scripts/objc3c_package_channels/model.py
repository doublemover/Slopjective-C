"""Package channel data model and payload builders."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
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
MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"


@dataclass(frozen=True)
class PackageChannelSurfaceInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelInputs:
    supported_platforms: dict[str, Any]
    metadata_surface: dict[str, Any]
    platform_support_matrix: dict[str, Any]


@dataclass(frozen=True)
class PackageChannelPaths:
    run_id: str
    package_root: Path
    build_root: Path
    portable_archive: Path
    installer_image_root: Path
    installer_archive: Path
    offline_bundle_root: Path
    offline_archive: Path
    manifest_path: Path


def package_channel_paths(run_id: str) -> PackageChannelPaths:
    package_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels" / run_id / "runnable"
    build_root = ARTIFACT_ROOT / run_id / "windows-x64"
    portable_archive = build_root / "portable" / "objc3c-windows-x64-portable.zip"
    installer_image_root = build_root / "installer" / "image"
    installer_archive = build_root / "installer" / "objc3c-windows-x64-installer.zip"
    offline_bundle_root = build_root / "offline" / "bundle"
    offline_archive = build_root / "offline" / "objc3c-windows-x64-offline-bundle.zip"
    manifest_path = build_root / "objc3c-package-channels-manifest.json"
    return PackageChannelPaths(
        run_id=run_id,
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
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.packaging.channels.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": inputs.supported_platforms["default_platform_id"],
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
    }
