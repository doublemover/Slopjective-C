"""Command result output helpers."""

from __future__ import annotations

import sys

from .command_result_model import WorkflowCommandResult


def emit_result_error(result: WorkflowCommandResult) -> None:
    if result.message:
        print(result.message, file=sys.stderr)
