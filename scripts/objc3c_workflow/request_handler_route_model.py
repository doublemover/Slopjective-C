"""Typed route model for parsed workflow request handlers."""

from __future__ import annotations

from collections.abc import Callable
from dataclasses import dataclass


REQUEST_HANDLER_ROUTE_OWNER = "objc3c-workflow-request-handler-routes"
REQUEST_HANDLER_UNHANDLED_OWNER = "objc3c-workflow-request-handler-unhandled"


@dataclass(frozen=True)
class WorkflowRequestHandlerRoute:
    request_type: type[object]
    handler: Callable[[object], int]
    owner: str = REQUEST_HANDLER_ROUTE_OWNER


__all__ = [
    "REQUEST_HANDLER_ROUTE_OWNER",
    "REQUEST_HANDLER_UNHANDLED_OWNER",
    "WorkflowRequestHandlerRoute",
]
