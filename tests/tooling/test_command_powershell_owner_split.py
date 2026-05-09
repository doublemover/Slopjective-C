from __future__ import annotations

from pathlib import Path

from scripts.objc3c_workflow.command_powershell_policy import (
    POWERSHELL_NONINTERACTIVE_ARGS,
    powershell_file_command,
)


def test_powershell_file_command_uses_noninteractive_policy() -> None:
    command = powershell_file_command("pwsh", Path("scripts/check.ps1"), "-Flag")

    assert POWERSHELL_NONINTERACTIVE_ARGS == (
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
    )
    assert command == [
        "pwsh",
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        "scripts\\check.ps1",
        "-Flag",
    ]
