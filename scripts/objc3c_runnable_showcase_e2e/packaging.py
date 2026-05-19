from __future__ import annotations

from pathlib import Path
from typing import Any

from .constants import PACKAGE_PS1, PWSH, ROOT
from .tooling import run_capture


def run_package_command(*, package_root: Path) -> Any:
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


__all__ = ["run_package_command"]
