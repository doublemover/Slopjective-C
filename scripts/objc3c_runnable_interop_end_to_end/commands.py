"""Command execution helpers for runnable interop end-to-end validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from .paths import PACKAGE_PS1
from .paths import PWSH
from .paths import ROOT


def package_runnable_toolchain(package_root: Path) -> object:
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


def run_packaged_execution_smoke(smoke_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        cwd=cwd,
    )


def run_packaged_execution_replay(replay_script: Path, *, cwd: Path) -> object:
    return run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(replay_script),
            "-CaseId",
            "canonical-runnable",
        ],
        cwd=cwd,
    )


__all__ = [
    "package_runnable_toolchain",
    "run_packaged_execution_replay",
    "run_packaged_execution_smoke",
]
