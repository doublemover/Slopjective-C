"""Action-backed workflow request handlers."""

from __future__ import annotations

from .action_dispatch import (
    describe_action_payload,
    execute_registered_action,
    list_actions_payload,
)
from .argument_request_model import (
    DescribeActionRequest,
    ExecuteActionRequest,
    ListActionsRequest,
)
from .registry_views import has_action
from .reports import emit_json
from .request_unknown_errors import emit_unknown_action


def handle_list_actions_request(_: ListActionsRequest) -> int:
    return emit_json(list_actions_payload())


def handle_describe_action_request(request: DescribeActionRequest) -> int:
    if not has_action(request.action):
        return emit_unknown_action(request.action)
    return emit_json(describe_action_payload(request.action))


def handle_execute_action_request(request: ExecuteActionRequest) -> int:
    return execute_registered_action(request.action, request.args)


__all__ = [
    "handle_describe_action_request",
    "handle_execute_action_request",
    "handle_list_actions_request",
]
