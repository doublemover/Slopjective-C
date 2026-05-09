"""Owned entrypoint dispatch policy for objc3c workflow CLIs."""

from __future__ import annotations

from collections.abc import Callable, Sequence

WORKFLOW_ENTRYPOINT_POLICY_OWNER = "objc3c-workflow-entrypoint-policy"
WORKFLOW_MODULE_ENTRYPOINT_OWNER = "objc3c-workflow-module-entrypoint"
WORKFLOW_SCRIPT_ENTRYPOINT_OWNER = "objc3c-workflow-script-entrypoint"


def dispatch_entrypoint(
    dispatcher: Callable[[Sequence[str] | None], int],
    argv: Sequence[str] | None,
) -> int:
    return dispatcher(argv)


__all__ = [
    "WORKFLOW_ENTRYPOINT_POLICY_OWNER",
    "WORKFLOW_MODULE_ENTRYPOINT_OWNER",
    "WORKFLOW_SCRIPT_ENTRYPOINT_OWNER",
    "dispatch_entrypoint",
]
