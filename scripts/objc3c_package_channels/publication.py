"""Artifact assembly and publication for package channels."""

from __future__ import annotations

import shutil
import stat
import zipfile
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file, write_text_file as write_text
from objc3c_tooling.paths import repo_rel

from .model import (
    PackageChannelInputs,
    PackageChannelPaths,
    package_channels_report_payload,
    target_platform_id_for_paths,
)
from .paths import (
    ARTIFACT_ROOT,
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    REPORT_PATH,
    ROOT,
)
from .rendering import (
    bootstrap_script_text,
    install_script_text,
    offline_bootstrap_script_text,
    uninstall_script_text,
)


OWNED_PACKAGE_RUN_ROOT = ROOT / "tmp" / "pkg" / "objc3c-package-channels"
OWNED_CLEAN_ROOTS = (OWNED_PACKAGE_RUN_ROOT, ARTIFACT_ROOT)


def prepare_package_channel_workspace(
    paths: PackageChannelPaths,
    *,
    preserve_package_root: bool = False,
) -> None:
    if not preserve_package_root:
        remove_owned_tree(paths.package_root.parent)
    remove_owned_tree(paths.build_root)


def remove_owned_tree(path: Path) -> None:
    assert_owned_cleanup_path(path)
    if not path.exists() and not path.is_symlink():
        return
    assert_tree_has_no_reparse_points(path)
    if path.is_dir():
        shutil.rmtree(path)
    else:
        path.unlink()


def assert_owned_cleanup_path(path: Path) -> None:
    resolved_path = path.resolve(strict=False)
    for clean_root in OWNED_CLEAN_ROOTS:
        resolved_root = clean_root.resolve(strict=False)
        try:
            relative = resolved_path.relative_to(resolved_root)
        except ValueError:
            continue
        if not relative.parts:
            raise RuntimeError(f"refusing to clean package-channel root itself: {repo_rel(path)}")
        return
    raise RuntimeError(f"refusing to clean path outside package-channel owned roots: {path}")


def assert_tree_has_no_reparse_points(path: Path) -> None:
    if path.is_symlink() or is_junction(path):
        raise RuntimeError(f"refusing to clean package-channel reparse point: {path}")
    candidates: list[Path] = []
    if path.is_dir():
        candidates.extend(path.rglob("*"))
    for candidate in candidates:
        if candidate.is_symlink() or is_junction(candidate):
            raise RuntimeError(f"refusing to clean package-channel reparse point: {candidate}")


def is_junction(path: Path) -> bool:
    is_junction_method = getattr(path, "is_junction", None)
    return bool(is_junction_method and is_junction_method())


def zip_directory(source_dir: Path, destination_zip: Path) -> None:
    destination_zip.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(destination_zip, mode="w", compression=zipfile.ZIP_DEFLATED) as archive:
        for file_path in sorted(path for path in source_dir.rglob("*") if path.is_file()):
            arcname = str(file_path.relative_to(source_dir)).replace("\\", "/")
            zip_info = zipfile.ZipInfo.from_file(file_path, arcname=arcname)
            zip_info.compress_type = zipfile.ZIP_DEFLATED
            mode = stat.S_IMODE(file_path.stat().st_mode) or 0o644
            if "/artifacts/bin/" in f"/{arcname}":
                mode = (mode | stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH) & 0o777
            zip_info.external_attr = (stat.S_IFREG | mode) << 16
            with file_path.open("rb") as source:
                with archive.open(zip_info, "w") as target:
                    shutil.copyfileobj(source, target)


def publish_portable_archive(paths: PackageChannelPaths) -> None:
    zip_directory(paths.package_root, paths.portable_archive)


def publish_installer_archive(paths: PackageChannelPaths) -> None:
    target_platform_id = target_platform_id_for_paths(paths)
    installer_payload_root = paths.installer_image_root / "payload"
    shutil.copytree(paths.package_root, installer_payload_root, dirs_exist_ok=True)
    write_text(
        paths.installer_image_root / "Install-objc3c.ps1",
        install_script_text(
            paths.sanitizer_variant,
            target_platform_id=target_platform_id,
        ),
    )
    write_text(
        paths.installer_image_root / "Uninstall-objc3c.ps1",
        uninstall_script_text(target_platform_id=target_platform_id),
    )
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
    write_text(
        paths.offline_bundle_root / "OfflineBootstrap-objc3c.ps1",
        offline_bootstrap_script_text(
            paths.sanitizer_variant,
            paths.installer_archive.name,
        ),
    )
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
