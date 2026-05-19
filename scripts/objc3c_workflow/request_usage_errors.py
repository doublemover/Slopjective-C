"""Usage-error diagnostics for workflow requests."""

from __future__ import annotations

from .argument_usage_error import WorkflowUsageError
from .request_error_output import emit_request_error


def emit_usage_error(exc: WorkflowUsageError) -> int:
    return emit_request_error(exc.message, exc.exit_code)


__all__ = ["emit_usage_error"]
