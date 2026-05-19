"""Parsed workflow request dispatch."""

from __future__ import annotations

from collections.abc import Sequence

from .request_handler_dispatch import dispatch_parsed_workflow_request
from .argument_requests import WorkflowRequest
from .request_parse_dispatch import parse_and_dispatch_workflow_request


def dispatch_workflow_request(request: WorkflowRequest) -> int:
    return dispatch_parsed_workflow_request(request)


def dispatch_workflow_argv(argv: Sequence[str]) -> int:
    return parse_and_dispatch_workflow_request(argv)


__all__ = [
    "dispatch_workflow_argv",
    "dispatch_workflow_request",
    "parse_and_dispatch_workflow_request",
]
