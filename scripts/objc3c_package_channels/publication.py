"""Artifact assembly and publication for package channels."""

from __future__ import annotations

import shutil
import zipfile
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file, write_text_file as write_text
from objc3c_tooling.paths import repo_rel

from .model import PackageChannelInputs, PackageChannelPaths, package_channels_report_payload
from .paths import (
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    REPORT_PATH,
)
from .rendering import (
    bootstrap_script_text,
    install_script_text,
    offline_bootstrap_script_text,
    uninstall_script_text,
)


def zip_directory(source_dir: Path, destination_zip: Path) -> None:
    destination_zip.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(destination_zip, mode="w", compression=zipfile.ZIP_DEFLATED) as archive:
        for file_path in sorted(path for path in source_dir.rglob("*") if path.is_file()):
            archive.write(file_path, arcname=str(file_path.relative_to(source_dir)).replace("\\", "/"))


def publish_portable_archive(paths: PackageChannelPaths) -> None:
    zip_directory(paths.package_root, paths.portable_archive)


def publish_installer_archive(paths: PackageChannelPaths) -> None:
    installer_payload_root = paths.installer_image_root / "payload"
    shutil.copytree(paths.package_root, installer_payload_root, dirs_exist_ok=True)
    write_text(paths.installer_image_root / "Install-objc3c.ps1", install_script_text())
    write_text(paths.installer_image_root / "Uninstall-objc3c.ps1", uninstall_script_text())
    write_text(paths.installer_image_root / "Bootstrap-objc3cEnvironment.ps1", bootstrap_script_text())
    zip_directory(paths.installer_image_root, paths.installer_archive)


def publish_offline_bundle(paths: PackageChannelPaths) -> None:
    channels_root = paths.offline_bundle_root / "channels"
    evidence_root = paths.offline_bundle_root / "release-foundation"
    channels_root.mkdir(parents=True, exist_ok=True)
    evidence_root.mkdir(parents=True, exist_ok=True)
    shutil.copy2(paths.portable_archive, channels_root / paths.portable_archive.name)
    shutil.copy2(paths.installer_archive, channels_root / paths.installer_archive.name)
    shutil.copy2(RELEASE_FOUNDATION_MANIFEST, evidence_root / RELEASE_FOUNDATION_MANIFEST.name)
    shutil.copy2(RELEASE_FOUNDATION_SBOM, evidence_root / RELEASE_FOUNDATION_SBOM.name)
    shutil.copy2(RELEASE_FOUNDATION_ATTESTATION, evidence_root / RELEASE_FOUNDATION_ATTESTATION.name)
    write_text(paths.offline_bundle_root / "OfflineBootstrap-objc3c.ps1", offline_bootstrap_script_text())
    zip_directory(paths.offline_bundle_root, paths.offline_archive)


def write_package_channel_artifacts(
    *,
    inputs: PackageChannelInputs,
    paths: PackageChannelPaths,
    manifest_payload: dict[str, Any],
) -> dict[str, Any]:
    paths.manifest_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(paths.manifest_path, manifest_payload)

    report_payload = package_channels_report_payload(
        inputs=inputs,
        paths=paths,
        manifest_payload=manifest_payload,
    )
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, report_payload)
    return report_payload


def print_package_channel_result(paths: PackageChannelPaths) -> None:
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"portable_archive: {repo_rel(paths.portable_archive)}")
    print("objc3c-package-channels: PASS")
