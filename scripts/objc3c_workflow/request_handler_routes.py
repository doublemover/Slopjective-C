"""Owned route table for parsed workflow request handlers."""

from __future__ import annotations

from collections.abc import Callable

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
from .request_handler_route_model import (
    REQUEST_HANDLER_ROUTE_OWNER,
    REQUEST_HANDLER_UNHANDLED_OWNER,
    WorkflowRequestHandlerRoute,
)


REQUEST_HANDLER_ROUTES: tuple[WorkflowRequestHandlerRoute, ...] = (
    WorkflowRequestHandlerRoute(ListActionsRequest, handle_list_actions_request),
    WorkflowRequestHandlerRoute(DescribeActionRequest, handle_describe_action_request),
    WorkflowRequestHandlerRoute(
        DescribePackageScriptRequest,
        handle_describe_package_script_request,
    ),
    WorkflowRequestHandlerRoute(ExecuteActionRequest, handle_execute_action_request),
)


def handler_for_request(request: WorkflowRequest) -> Callable[[object], int] | None:
    for route in REQUEST_HANDLER_ROUTES:
        if isinstance(request, route.request_type):
            return route.handler
    return None


def unhandled_workflow_request_message(request: WorkflowRequest) -> str:
    return f"unhandled workflow request: {request!r}"


__all__ = [
    "REQUEST_HANDLER_ROUTE_OWNER",
    "REQUEST_HANDLER_ROUTES",
    "REQUEST_HANDLER_UNHANDLED_OWNER",
    "WorkflowRequestHandlerRoute",
    "handler_for_request",
    "unhandled_workflow_request_message",
]
