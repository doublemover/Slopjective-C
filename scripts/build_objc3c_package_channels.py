#!/usr/bin/env python3
"""Build objc3c portable archive and installer-shaped package channels."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import zipfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "source_surface.json"
SUPPORTED_PLATFORMS = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "supported_platforms.json"
INSTALLER_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "installer_policy.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "package-channels"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run(command: list[str]) -> None:
    env = os.environ.copy()
    env["PYTHONDONTWRITEBYTECODE"] = "1"
    result = subprocess.run(command, cwd=ROOT, env=env, text=True, capture_output=True, check=False)
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def zip_directory(source_dir: Path, destination_zip: Path) -> None:
    destination_zip.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(destination_zip, mode="w", compression=zipfile.ZIP_DEFLATED) as archive:
        for file_path in sorted(path for path in source_dir.rglob("*") if path.is_file()):
            archive.write(file_path, arcname=str(file_path.relative_to(source_dir)).replace("\\", "/"))


def main() -> int:
    load_json(SOURCE_SURFACE)
    supported_platforms = load_json(SUPPORTED_PLATFORMS)
    load_json(INSTALLER_POLICY)

    run([sys.executable, str(RELEASE_MANIFEST_PY)])
    run([sys.executable, str(RELEASE_PROVENANCE_PY)])

    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-package-channels" / run_id / "runnable"
    manifest_relative_path = "artifacts/package/objc3c-runnable-toolchain-package.json"
    run([
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
    ])

    build_root = ARTIFACT_ROOT / run_id / "windows-x64"
    portable_root = build_root / "portable"
    portable_archive = portable_root / "objc3c-windows-x64-portable.zip"
    zip_directory(package_root, portable_archive)

    payload = {
        "contract_id": "objc3c.packaging.channels.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": supported_platforms["default_platform_id"],
        "package_root": repo_rel(package_root),
        "portable_archive": repo_rel(portable_archive),
        "implemented_channels": ["portable-archive"],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"portable_archive: {repo_rel(portable_archive)}")
    print("objc3c-package-channels: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
