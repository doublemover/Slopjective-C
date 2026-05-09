"""Typed workflow request handlers."""

from __future__ import annotations

from .action_dispatch import (
    describe_action_payload,
    execute_registered_action,
    list_actions_payload,
)
from .arguments import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowRequest,
)
from .npm_surface import describe_package_script_payload
from .public_bridge import PACKAGE_BRIDGES
from .registry_views import has_action
from .reports import emit_json
from .request_errors import emit_unknown_action, emit_unknown_package_script


def handle_list_actions_request(_: ListActionsRequest) -> int:
    return emit_json(list_actions_payload())


def handle_describe_action_request(request: DescribeActionRequest) -> int:
    if not has_action(request.action):
        return emit_unknown_action(request.action)
    return emit_json(describe_action_payload(request.action))


def handle_describe_package_script_request(
    request: DescribePackageScriptRequest,
) -> int:
    if request.package_script not in PACKAGE_BRIDGES:
        return emit_unknown_package_script(request.package_script)
    return emit_json(describe_package_script_payload(request.package_script))


def handle_execute_action_request(request: ExecuteActionRequest) -> int:
    return execute_registered_action(request.action, request.args)


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
