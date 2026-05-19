"""Path constants for package channel publication."""

from __future__ import annotations

import shutil
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"

PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
RELEASE_MANIFEST_PY = ROOT / "scripts" / "build_objc3c_release_manifest.py"
RELEASE_PROVENANCE_PY = ROOT / "scripts" / "publish_objc3c_release_provenance.py"
PLATFORM_SUPPORT_MATRIX_BUILD = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"

SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "source_surface.json"
SUPPORTED_PLATFORMS = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "supported_platforms.json"
INSTALLER_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "installer_policy.json"
METADATA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "metadata_surface.json"
SCHEMA_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "packaging_channels" / "schema_surface.json"

PLATFORM_SUPPORT_MATRIX_ARTIFACT = (
    ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
)
PLATFORM_SUPPORT_MATRIX_SUMMARY = (
    ROOT / "tmp" / "reports" / "platform-hardening" / "platform-support-matrix-summary.json"
)
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "package-channels"
REPORT_PATH = ROOT / "tmp" / "reports" / "package-channels" / "package-channels-summary.json"

RELEASE_FOUNDATION_MANIFEST = (
    ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
)
RELEASE_FOUNDATION_SBOM = ROOT / "tmp" / "artifacts" / "release-foundation" / "sbom" / "objc3c-release-sbom.json"
RELEASE_FOUNDATION_ATTESTATION = (
    ROOT / "tmp" / "artifacts" / "release-foundation" / "attestation" / "objc3c-release-attestation.json"
)


def package_channel_run_id() -> str:
    return datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
