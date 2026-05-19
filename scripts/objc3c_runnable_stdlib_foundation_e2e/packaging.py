from __future__ import annotations

from pathlib import Path

from objc3c_tooling.subprocesses import run_capture

from .constants import PACKAGE_PS1, PWSH, ROOT


def run_package_command(*, package_root: Path) -> object:
    package_result = run_capture(
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
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")
    return package_result
