"""Open-blocker audit command execution helpers."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path
from objc3c_tooling.public_workflow_output import normalize_newlines
from objc3c_tooling.subprocesses import python_script_command, run_timed

from .audit_runner_constants import DEFAULT_COMMAND_TIMEOUT_SECONDS, EXIT_RUNNER_ERROR
from .audit_runner_models import CommandResult, CommandSpec


def run_command(spec: CommandSpec, *, root: Path) -> CommandResult:
    command = python_script_command(spec.script_path, *spec.actual_args)
    execution = run_timed(command, cwd=root, timeout=DEFAULT_COMMAND_TIMEOUT_SECONDS)
    exit_code = (
        EXIT_RUNNER_ERROR
        if execution.timeout_seconds is not None
        else int(execution.returncode)
    )
    return CommandResult(
        spec=spec,
        exit_code=exit_code,
        stdout=normalize_newlines(execution.stdout),
        stderr=normalize_newlines(execution.stderr),
    )


def summarize_command(result: CommandResult) -> dict[str, Any]:
    return {
        "argv": python_script_command(
            display_path(result.spec.script_path),
            *result.spec.display_args,
        ),
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }
