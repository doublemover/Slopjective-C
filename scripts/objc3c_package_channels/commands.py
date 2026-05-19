"""Subprocess boundaries for package channel publication."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.subprocesses import python_script_command, run_completed
from scripts.objc3c_workflow.command_powershell_policy import powershell_file_command

from .paths import (
    PACKAGE_PS1,
    PLATFORM_SUPPORT_MATRIX_BUILD,
    PWSH,
    RELEASE_MANIFEST_PY,
    RELEASE_PROVENANCE_PY,
    ROOT,
)


def run(command: list[str]) -> None:
    result = run_completed(command, cwd=ROOT, capture_output=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def build_support_matrix() -> None:
    run(python_script_command(PLATFORM_SUPPORT_MATRIX_BUILD))


def build_release_foundation_artifacts() -> None:
    run(python_script_command(RELEASE_MANIFEST_PY))
    run(python_script_command(RELEASE_PROVENANCE_PY))


def build_runnable_package(package_root: Path, manifest_relative_path: str) -> None:
    run(
        powershell_file_command(
            PWSH,
            PACKAGE_PS1,
            "-PackageRoot",
            str(package_root),
            "-ManifestRelativePath",
            manifest_relative_path,
        )
    )
