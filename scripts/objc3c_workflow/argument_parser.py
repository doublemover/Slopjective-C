"""Argument parser for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence

from .argument_request_model import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowRequest,
)
from .argument_usage_error import WorkflowUsageError
from .argument_usage import usage_text
from .environment import WORKFLOW_COMMAND_TEXT


def parse_workflow_args(argv: Sequence[str]) -> WorkflowRequest:
    args = list(argv)
    if not args:
        raise WorkflowUsageError(usage_text())

    action, *rest = args
    if action == "--list-json":
        return ListActionsRequest()
    if action == "--describe":
        if len(rest) != 1:
            raise WorkflowUsageError(f"usage: {WORKFLOW_COMMAND_TEXT} --describe <action>")
        return DescribeActionRequest(rest[0])
    if action == "--describe-script":
        if len(rest) != 1:
            raise WorkflowUsageError(
                f"usage: {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>"
            )
        return DescribePackageScriptRequest(rest[0])
    return ExecuteActionRequest(action, rest)


__all__ = ["parse_workflow_args"]
