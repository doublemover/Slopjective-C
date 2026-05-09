"""Parsed workflow request dispatch."""

from __future__ import annotations

import sys
from collections.abc import Sequence

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
    WorkflowUsageError,
    parse_workflow_args,
)
from .npm_surface import describe_package_script_payload
from .public_bridge import PACKAGE_BRIDGES
from .registry_views import has_action
from .reports import emit_json


def dispatch_workflow_request(request: WorkflowRequest) -> int:
    if isinstance(request, ListActionsRequest):
        return emit_json(list_actions_payload())
    if isinstance(request, DescribeActionRequest):
        if not has_action(request.action):
            print(f"unknown action: {request.action}", file=sys.stderr)
            return 2
        return emit_json(describe_action_payload(request.action))
    if isinstance(request, DescribePackageScriptRequest):
        if request.package_script not in PACKAGE_BRIDGES:
            print(
                f"unknown package script: {request.package_script}",
                file=sys.stderr,
            )
            return 2
        return emit_json(describe_package_script_payload(request.package_script))
    if isinstance(request, ExecuteActionRequest):
        return execute_registered_action(request.action, request.args)
    raise AssertionError(f"unhandled workflow request: {request!r}")


def parse_and_dispatch_workflow_request(argv: Sequence[str]) -> int:
    try:
        request = parse_workflow_args(argv)
    except WorkflowUsageError as exc:
        print(exc.message, file=sys.stderr)
        return exc.exit_code
    return dispatch_workflow_request(request)
