"""Parsed workflow request dispatch."""

from __future__ import annotations

from collections.abc import Sequence

from .arguments import WorkflowRequest, WorkflowUsageError, parse_workflow_args
from .request_errors import emit_usage_error
from .request_handlers import dispatch_parsed_workflow_request


def dispatch_workflow_request(request: WorkflowRequest) -> int:
    return dispatch_parsed_workflow_request(request)


def parse_and_dispatch_workflow_request(argv: Sequence[str]) -> int:
    try:
        request = parse_workflow_args(argv)
    except WorkflowUsageError as exc:
        return emit_usage_error(exc)
    return dispatch_workflow_request(request)
