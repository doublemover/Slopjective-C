"""Parsed workflow request dispatch owner."""

from __future__ import annotations

from .argument_request_model import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowRequest,
)
from .request_handler_actions import (
    handle_describe_action_request,
    handle_execute_action_request,
    handle_list_actions_request,
)
from .request_handler_package_scripts import handle_describe_package_script_request


def dispatch_parsed_workflow_request(request: WorkflowRequest) -> int:
    if isinstance(request, ListActionsRequest):
        return handle_list_actions_request(request)
    if isinstance(request, DescribeActionRequest):
        return handle_describe_action_request(request)
    if isinstance(request, DescribePackageScriptRequest):
        return handle_describe_package_script_request(request)
    if isinstance(request, ExecuteActionRequest):
        return handle_execute_action_request(request)
    raise AssertionError(f"unhandled workflow request: {request!r}")


__all__ = ["dispatch_parsed_workflow_request"]
