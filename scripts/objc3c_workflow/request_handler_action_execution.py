"""Action execution request handler."""

from __future__ import annotations

from .action_execution_dispatch import execute_registered_action
from .argument_request_model import ExecuteActionRequest


def handle_execute_action_request(request: ExecuteActionRequest) -> int:
    return execute_registered_action(request.action, request.args)


__all__ = ["handle_execute_action_request"]
