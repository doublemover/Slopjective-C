from __future__ import annotations

import subprocess
from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class CommandResult:
    role: str
    command: list[str]
    exit_code: int
    stdout: str
    stderr: str


def format_command(command: Sequence[str]) -> str:
    return subprocess.list2cmdline([str(part) for part in command])


def run_command(role: str, command: Sequence[str]) -> CommandResult:
    completed = subprocess.run(
        [str(part) for part in command],
        capture_output=True,
        text=True,
        check=False,
    )
    return CommandResult(
        role=role,
        command=[str(part) for part in command],
        exit_code=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )


