#!/usr/bin/env python3
"""Build objc3c portable archive and installer-shaped package channels."""

from __future__ import annotations

import json
import os
import re
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
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "schema_surface.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "package-channels"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"
RELEASE_FOUNDATION_MANIFEST = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
RELEASE_FOUNDATION_SBOM = ROOT / "tmp" / "artifacts" / "release-foundation" / "sbom" / "objc3c-release-sbom.json"
RELEASE_FOUNDATION_ATTESTATION = ROOT / "tmp" / "artifacts" / "release-foundation" / "attestation" / "objc3c-release-attestation.json"


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


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.replace("\r\n", "\n"), encoding="utf-8")


def powershell_single_quote(text: str) -> str:
    return text.replace("'", "''")


def install_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot,
  [switch]$Force
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$sourceRoot = Join-Path $PSScriptRoot "payload"
$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapSource = Join-Path $PSScriptRoot "Bootstrap-objc3cEnvironment.ps1"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"

if ((Test-Path -LiteralPath $installHome) -and -not $Force.IsPresent) {
  throw "installer target already exists: $installHome"
}

if (Test-Path -LiteralPath $installHome) {
  Remove-Item -LiteralPath $installHome -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $resolvedInstallRoot | Out-Null
Copy-Item -LiteralPath $sourceRoot -Destination $installHome -Recurse -Force
Copy-Item -LiteralPath $bootstrapSource -Destination $bootstrapTarget -Force

$receipt = [ordered]@{
  contract_id = "objc3c.packaging.channels.install-receipt.v1"
  install_root = $resolvedInstallRoot
  install_home = $installHome
  bootstrap_script = $bootstrapTarget
  installed_at_utc = [DateTime]::UtcNow.ToString("o")
}
$receipt | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $receiptPath -Encoding utf8

Write-Output ("install_root: " + $resolvedInstallRoot)
Write-Output ("install_home: " + $installHome)
Write-Output ("receipt_path: " + $receiptPath)
Write-Output ("bootstrap_script: " + $bootstrapTarget)
"""


def uninstall_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"

if (Test-Path -LiteralPath $installHome) {
  Remove-Item -LiteralPath $installHome -Recurse -Force
}
if (Test-Path -LiteralPath $bootstrapTarget) {
  Remove-Item -LiteralPath $bootstrapTarget -Force
}
if (Test-Path -LiteralPath $receiptPath) {
  Remove-Item -LiteralPath $receiptPath -Force
}

Write-Output ("rollback_root: " + $resolvedInstallRoot)
"""


def bootstrap_script_text() -> str:
    return """param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$installRoot = Split-Path -Parent $PSScriptRoot
$toolchainHome = Join-Path $installRoot "objc3c"
$binPath = Join-Path $toolchainHome "artifacts/bin"

$env:OBJC3C_HOME = $toolchainHome
if ($env:PATH -notmatch [regex]::Escape($binPath)) {
  $env:PATH = $binPath + [System.IO.Path]::PathSeparator + $env:PATH
}

Write-Output ("objc3c_home: " + $env:OBJC3C_HOME)
Write-Output ("objc3c_bin: " + $binPath)
"""


def offline_bootstrap_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$bundleRoot = $PSScriptRoot
$stagingRoot = Join-Path $bundleRoot "staging"
$installerArchive = Join-Path $bundleRoot "channels/objc3c-windows-x64-installer.zip"
$installerImageRoot = Join-Path $stagingRoot "installer-image"
$installerScript = Join-Path $installerImageRoot "Install-objc3c.ps1"

if (Test-Path -LiteralPath $installerImageRoot) {
  Remove-Item -LiteralPath $installerImageRoot -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $stagingRoot | Out-Null
Expand-Archive -LiteralPath $installerArchive -DestinationPath $installerImageRoot -Force

& $installerScript -InstallRoot $InstallRoot -Force
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

Write-Output ("offline_bundle_root: " + $bundleRoot)
Write-Output ("installer_archive: " + $installerArchive)
"""


def main() -> int:
    load_json(SOURCE_SURFACE)
    supported_platforms = load_json(SUPPORTED_PLATFORMS)
    load_json(INSTALLER_POLICY)
    metadata_surface = load_json(METADATA_SURFACE)
    load_json(SCHEMA_SURFACE)

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

    installer_root = build_root / "installer"
    installer_image_root = installer_root / "image"
    installer_payload_root = installer_image_root / "payload"
    shutil.copytree(package_root, installer_payload_root, dirs_exist_ok=True)
    write_text(installer_image_root / "Install-objc3c.ps1", install_script_text())
    write_text(installer_image_root / "Uninstall-objc3c.ps1", uninstall_script_text())
    write_text(installer_image_root / "Bootstrap-objc3cEnvironment.ps1", bootstrap_script_text())
    installer_archive = installer_root / "objc3c-windows-x64-installer.zip"
    zip_directory(installer_image_root, installer_archive)

    offline_root = build_root / "offline"
    offline_bundle_root = offline_root / "bundle"
    channels_root = offline_bundle_root / "channels"
    evidence_root = offline_bundle_root / "release-foundation"
    channels_root.mkdir(parents=True, exist_ok=True)
    evidence_root.mkdir(parents=True, exist_ok=True)
    shutil.copy2(portable_archive, channels_root / portable_archive.name)
    shutil.copy2(installer_archive, channels_root / installer_archive.name)
    shutil.copy2(RELEASE_FOUNDATION_MANIFEST, evidence_root / RELEASE_FOUNDATION_MANIFEST.name)
    shutil.copy2(RELEASE_FOUNDATION_SBOM, evidence_root / RELEASE_FOUNDATION_SBOM.name)
    shutil.copy2(RELEASE_FOUNDATION_ATTESTATION, evidence_root / RELEASE_FOUNDATION_ATTESTATION.name)
    write_text(offline_bundle_root / "OfflineBootstrap-objc3c.ps1", offline_bootstrap_script_text())
    offline_archive = offline_root / "objc3c-windows-x64-offline-bundle.zip"
    zip_directory(offline_bundle_root, offline_archive)

    manifest_payload = {
        "contract_id": "objc3c.packaging.channels.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": supported_platforms["default_platform_id"],
        "package_root": repo_rel(package_root),
        "portable_archive": repo_rel(portable_archive),
        "installer_archive": repo_rel(installer_archive),
        "offline_archive": repo_rel(offline_archive),
        "installer_image_root": repo_rel(installer_image_root),
        "offline_bundle_root": repo_rel(offline_bundle_root),
        "implemented_channels": ["portable-archive", "local-installer", "offline-bundle"],
        "release_foundation_artifacts": {
            "manifest": repo_rel(RELEASE_FOUNDATION_MANIFEST),
            "sbom": repo_rel(RELEASE_FOUNDATION_SBOM),
            "attestation": repo_rel(RELEASE_FOUNDATION_ATTESTATION),
        },
    }
    for field_name in metadata_surface["required_manifest_fields"]:
        if field_name not in manifest_payload:
            raise RuntimeError(f"package-channels manifest missing required field {field_name}")
    manifest_path = build_root / "objc3c-package-channels-manifest.json"
    write_text(manifest_path, json.dumps(manifest_payload, indent=2) + "\n")

    payload = {
        "contract_id": "objc3c.packaging.channels.summary.report.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "status": "PASS",
        "platform_id": supported_platforms["default_platform_id"],
        "package_root": repo_rel(package_root),
        "manifest_path": repo_rel(manifest_path),
        "portable_archive": repo_rel(portable_archive),
        "installer_image_root": repo_rel(installer_image_root),
        "installer_archive": repo_rel(installer_archive),
        "offline_bundle_root": repo_rel(offline_bundle_root),
        "offline_archive": repo_rel(offline_archive),
        "implemented_channels": manifest_payload["implemented_channels"],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"portable_archive: {repo_rel(portable_archive)}")
    print("objc3c-package-channels: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
