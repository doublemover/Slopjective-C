"""Command result output helpers."""

from __future__ import annotations

from .command_result_model import WorkflowCommandResult
from .command_result_policy import emit_command_result_message


def emit_result_error(result: WorkflowCommandResult) -> None:
    emit_command_result_message(result.message)
