"""PowerShell command helpers for objc3c workflow actions."""

from __future__ import annotations

from pathlib import Path

from .command_execution import run
from .command_powershell_policy import powershell_file_command
from .environment import PWSH


def pwsh_file(script: Path, *args: str) -> int:
    return run(powershell_file_command(PWSH, script, *args))


__all__ = ["pwsh_file"]
