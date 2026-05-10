"""Bootstrap readiness subprocess execution."""

from __future__ import annotations

from objc3c_tooling.public_workflow_output import normalize_newlines
from objc3c_tooling.subprocesses import python_script_command, run_timed

from .constants import DEFAULT_COMMAND_TIMEOUT_SECONDS, EXIT_RUNNER_ERROR, ROOT
from .models import CommandResult, CommandSpec


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
