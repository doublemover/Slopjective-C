"""Subprocess execution owner for objc3c workflow actions."""

from __future__ import annotations

from collections.abc import Sequence

from objc3c_tooling.subprocesses import run_completed

from .command_execution_policy import command_for_execution, workflow_command_subprocess_kwargs


def run(command: Sequence[str]) -> int:
    return run_completed(
        command_for_execution(command),
        **workflow_command_subprocess_kwargs(),
    ).returncode


__all__ = ["run"]
