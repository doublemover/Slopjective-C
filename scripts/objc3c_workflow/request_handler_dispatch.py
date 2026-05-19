"""Parsed workflow request dispatch owner."""

from __future__ import annotations

from .argument_request_model import WorkflowRequest
from .request_handler_routes import handler_for_request, unhandled_workflow_request_message


def dispatch_parsed_workflow_request(request: WorkflowRequest) -> int:
    handler = handler_for_request(request)
    if handler is None:
        raise AssertionError(unhandled_workflow_request_message(request))
    return handler(request)


__all__ = ["dispatch_parsed_workflow_request"]
