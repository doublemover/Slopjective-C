"""Command builders for public test orchestration profiles."""

from __future__ import annotations

import sys

from ..commands import workflow_command
from ..environment import PWSH
from .runtime_acceptance_routes import runtime_acceptance_command


def python_script(script: object, *args: str) -> list[str]:
    return [sys.executable, str(script), *args]


def pwsh_script(script: object, *args: str) -> list[str]:
    return [
        PWSH,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(script),
        *args,
    ]


def runtime_acceptance_step(action: str) -> list[str]:
    return runtime_acceptance_command(action)


def workflow_action(action: str, *args: str) -> list[str]:
    return workflow_command(action, *args)
