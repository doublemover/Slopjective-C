"""Parse argv and dispatch typed workflow requests."""

from __future__ import annotations

from collections.abc import Sequence

from .argument_parser import parse_workflow_args
from .argument_requests import WorkflowUsageError
from .request_handler_dispatch import dispatch_parsed_workflow_request
from .request_usage_errors import emit_usage_error


def parse_and_dispatch_workflow_request(argv: Sequence[str]) -> int:
    try:
        request = parse_workflow_args(argv)
    except WorkflowUsageError as exc:
        return emit_usage_error(exc)
    return dispatch_parsed_workflow_request(request)


__all__ = ["parse_and_dispatch_workflow_request"]
