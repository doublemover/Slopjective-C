"""Subprocess boundaries for package channel publication."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.subprocesses import python_script_command, run_completed
from scripts.objc3c_workflow.command_powershell_policy import powershell_file_command

from .paths import (
    PACKAGE_PS1,
    PLATFORM_SUPPORT_MATRIX_BUILD,
    PWSH,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
    RELEASE_FOUNDATION_ATTESTATION,
    RELEASE_FOUNDATION_MANIFEST,
    RELEASE_FOUNDATION_SBOM,
    ROOT,
)

RELEASE_FOUNDATION_INTEGRATION_SUMMARY = (
    ROOT / "tmp" / "reports" / "release-foundation" / "integration-summary.json"
)


def run(command: list[str]) -> None:
    result = run_completed(command, cwd=ROOT, capture_output=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def build_support_matrix() -> None:
    run(python_script_command(PLATFORM_SUPPORT_MATRIX_BUILD))


def require_existing_release_foundation_artifacts() -> None:
    missing_paths = [
        path
        for path in (
            RELEASE_FOUNDATION_INTEGRATION_SUMMARY,
            RELEASE_FOUNDATION_MANIFEST,
            RELEASE_FOUNDATION_SBOM,
            RELEASE_FOUNDATION_ATTESTATION,
        )
        if not path.is_file()
    ]
    if missing_paths:
        formatted = ", ".join(str(path) for path in missing_paths)
        raise RuntimeError(f"release foundation reuse requested but artifacts are missing: {formatted}")

    summary = load_json_object(RELEASE_FOUNDATION_INTEGRATION_SUMMARY)
    if summary.get("status") != "PASS":
        raise RuntimeError("release foundation reuse requested but integration summary did not pass")
    print("package-channels: reused validated release-foundation artifacts")


def build_release_foundation_artifacts(*, reuse_existing: bool = False) -> None:
    if reuse_existing:
        require_existing_release_foundation_artifacts()
        return
    run(python_script_command(RELEASE_MANIFEST_PY))
    run(python_script_command(RELEASE_PROVENANCE_PY))


def build_runnable_package(
    package_root: Path,
    manifest_relative_path: str,
    sanitizer_variant: str = "release",
) -> None:
    run(
        powershell_file_command(
            PWSH,
            PACKAGE_PS1,
            "-PackageRoot",
            str(package_root),
            "-ManifestRelativePath",
            manifest_relative_path,
            "-SanitizerVariant",
            sanitizer_variant,
        )
    )
