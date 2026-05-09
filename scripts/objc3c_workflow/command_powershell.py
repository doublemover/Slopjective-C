"""PowerShell command helpers for objc3c workflow actions."""

from __future__ import annotations

from pathlib import Path

from .command_execution import run
from .environment import PWSH


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])


__all__ = ["pwsh_file"]
