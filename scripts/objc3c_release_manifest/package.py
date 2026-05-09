"""Runnable package assembly loading for release manifest generation."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .commands import run
from .hashing import release_payload_digest, sha256_file
from .model import PackageAssembly, PayloadEntry, component_group_for_path
from .paths import PACKAGE_PS1, PWSH


def _require_manifest_fields(
    package_manifest: dict[str, Any],
    *,
    package_root: Path,
    manifest_relative_path: str,
    required_manifest_fields: tuple[str, ...],
) -> None:
    missing_fields = [
        field_name
        for field_name in required_manifest_fields
        if field_name not in package_manifest
    ]
    if missing_fields:
        raise RuntimeError(
            "runnable package manifest missed required fields: "
            + ", ".join(missing_fields)
        )
    if package_manifest.get("package_root") != repo_rel(package_root):
        raise RuntimeError("runnable package manifest package_root drifted from staged root")
    if package_manifest.get("manifest_artifact") != manifest_relative_path:
        raise RuntimeError("runnable package manifest artifact path drifted")


def load_package_assembly(
    *,
    package_root: Path,
    manifest_relative_path: str,
    required_manifest_fields: tuple[str, ...],
) -> PackageAssembly:
    manifest_path = package_root / Path(manifest_relative_path.replace("/", os.sep))
    if not manifest_path.is_file():
        raise RuntimeError(f"missing runnable package manifest {manifest_path}")

    package_manifest = load_json(manifest_path)
    _require_manifest_fields(
        package_manifest,
        package_root=package_root,
        manifest_relative_path=manifest_relative_path,
        required_manifest_fields=required_manifest_fields,
    )
    copied_files = package_manifest.get("copied_files")
    if not isinstance(copied_files, list) or not copied_files:
        raise RuntimeError("runnable package manifest did not contain copied_files")
    if package_manifest.get("copied_file_count") != len(copied_files):
        raise RuntimeError("runnable package manifest copied_file_count drifted")

    return _assembly_from_manifest(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest=package_manifest,
        copied_files=copied_files,
    )


def _assembly_from_manifest(
    *,
    package_root: Path,
    manifest_path: Path,
    package_manifest: dict[str, Any],
    copied_files: list[Any],
) -> PackageAssembly:
    copied_paths: list[str] = []
    for raw_path in copied_files:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError("runnable package manifest contained an invalid copied file entry")
        copied_paths.append(raw_path)

    entries: list[PayloadEntry] = []
    for raw_path in copied_paths:
        file_path = package_root / Path(raw_path.replace("/", os.sep))
        if not file_path.is_file():
            raise RuntimeError(f"runnable package referenced missing file {raw_path}")
        entries.append(
            PayloadEntry(
                path=raw_path,
                sha256=sha256_file(file_path),
                byte_count=file_path.stat().st_size,
                component_group=component_group_for_path(raw_path),
            )
        )
    entries.sort(key=lambda item: item.path)
    return PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest=package_manifest,
        entries=tuple(entries),
        payload_digest=release_payload_digest(entries),
    )


def package_once(
    package_root: Path,
    manifest_relative_path: str,
    required_manifest_fields: tuple[str, ...],
) -> PackageAssembly:
    package_root.mkdir(parents=True, exist_ok=True)
    run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
            "-ManifestRelativePath",
            manifest_relative_path,
        ]
    )
    return load_package_assembly(
        package_root=package_root,
        manifest_relative_path=manifest_relative_path,
        required_manifest_fields=required_manifest_fields,
    )
