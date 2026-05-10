"""Argument parser for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence

from .argument_option_requests import parse_workflow_option_request
from .argument_request_model import (
    ExecuteActionRequest,
    WorkflowRequest,
)
from .argument_usage_error import WorkflowUsageError
from .argument_usage import usage_text


def parse_workflow_args(argv: Sequence[str]) -> WorkflowRequest:
    args = list(argv)
    if not args:
        raise WorkflowUsageError(usage_text())

    action, *rest = args
    option_request = parse_workflow_option_request(action, rest)
    if option_request is not None:
        return option_request
    return ExecuteActionRequest(action, rest)


__all__ = ["parse_workflow_args"]
