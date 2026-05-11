"""Command execution helpers for runnable developer-tooling end-to-end validation."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.subprocesses import run_capture
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .constants import PACKAGE_PS1
from .constants import PWSH
from .constants import ROOT


def run_package_command(package_root: Path) -> Any:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )


def run_inspect_editor_tooling(source_path: Path, *, cwd: Path) -> Any:
    return run_capture(
        public_workflow_command("inspect-editor-tooling", str(source_path)),
        cwd=cwd,
    )


def run_format_objc3c(source_path: Path, *, cwd: Path) -> Any:
    return run_capture(
        public_workflow_command("format-objc3c", str(source_path)),
        cwd=cwd,
    )


def run_materialize_playground_workspace(source_path: Path, *, cwd: Path) -> Any:
    return run_capture(
        public_workflow_command("materialize-playground-workspace", str(source_path)),
        cwd=cwd,
    )


def run_validate_developer_tooling(*, cwd: Path) -> Any:
    return run_capture(
        public_workflow_command("validate-developer-tooling"),
        cwd=cwd,
    )


__all__ = [
    "run_format_objc3c",
    "run_inspect_editor_tooling",
    "run_materialize_playground_workspace",
    "run_package_command",
    "run_validate_developer_tooling",
]
