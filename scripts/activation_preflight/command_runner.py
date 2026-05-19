"""Activation preflight subprocess execution owner."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.public_workflow_output import normalize_newlines
from objc3c_tooling.subprocesses import python_script_command, run_timed

from scripts.activation_preflight.contracts import CommandResult, CommandSpec


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_COMMAND_TIMEOUT_SECONDS = 600
EXIT_RUNNER_ERROR = 2


def run_command(spec: CommandSpec) -> CommandResult:
    command = python_script_command(spec.script_path, *spec.actual_args)
    execution = run_timed(command, cwd=ROOT, timeout=DEFAULT_COMMAND_TIMEOUT_SECONDS)
    exit_code = EXIT_RUNNER_ERROR if execution.timeout_seconds is not None else int(execution.returncode)
    return CommandResult(
        spec=spec,
        exit_code=exit_code,
        stdout=normalize_newlines(execution.stdout),
        stderr=normalize_newlines(execution.stderr),
    )


__all__ = ["DEFAULT_COMMAND_TIMEOUT_SECONDS", "EXIT_RUNNER_ERROR", "ROOT", "run_command"]
