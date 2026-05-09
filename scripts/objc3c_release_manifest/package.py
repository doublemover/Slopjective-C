"""Runnable package assembly loading for release manifest generation."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json

from .commands import run
from .hashing import sha256_file, sha256_text
from .model import PackageAssembly, PayloadEntry, component_group_for_path
from .paths import PACKAGE_PS1, PWSH


def package_once(package_root: Path, manifest_relative_path: str) -> PackageAssembly:
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
    manifest_path = package_root / Path(manifest_relative_path.replace("/", os.sep))
    if not manifest_path.is_file():
        raise RuntimeError(f"missing runnable package manifest {manifest_path}")

    package_manifest = load_json(manifest_path)
    copied_files = package_manifest.get("copied_files")
    if not isinstance(copied_files, list) or not copied_files:
        raise RuntimeError("runnable package manifest did not contain copied_files")

    entries: list[PayloadEntry] = []
    for raw_path in copied_files:
        if not isinstance(raw_path, str) or not raw_path:
            raise RuntimeError("runnable package manifest contained an invalid copied file entry")
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
    payload_digest = sha256_text(
        "".join(
            f"{entry.path}|{entry.sha256}|{entry.byte_count}|{entry.component_group}\n"
            for entry in entries
        )
    )
    return PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest=package_manifest,
        entries=tuple(entries),
        payload_digest=payload_digest,
    )

