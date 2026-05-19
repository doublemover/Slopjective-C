"""PowerShell invocation policy for workflow command helpers."""

from __future__ import annotations

from pathlib import Path

POWERSHELL_NONINTERACTIVE_ARGS: tuple[str, ...] = (
    "-NoProfile",
    "-NonInteractive",
    "-ExecutionPolicy",
    "Bypass",
)


def powershell_file_command(pwsh: str, script: Path, *args: str) -> list[str]:
    return [
        pwsh,
        *POWERSHELL_NONINTERACTIVE_ARGS,
        "-File",
        str(script),
        *args,
    ]


__all__ = ["POWERSHELL_NONINTERACTIVE_ARGS", "powershell_file_command"]
