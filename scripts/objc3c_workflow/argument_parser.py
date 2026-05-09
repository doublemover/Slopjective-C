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
from .argument_option_contracts import (
    DESCRIBE_ACTION_OPTION,
    DESCRIBE_PACKAGE_SCRIPT_OPTION,
    LIST_ACTIONS_OPTION,
    workflow_argument_option_usage,
)
from .argument_usage_error import WorkflowUsageError
from .argument_usage import usage_text


def parse_workflow_args(argv: Sequence[str]) -> WorkflowRequest:
    args = list(argv)
    if not args:
        raise WorkflowUsageError(usage_text())

    action, *rest = args
    if action == LIST_ACTIONS_OPTION:
        if rest:
            raise WorkflowUsageError(workflow_argument_option_usage(action))
        return ListActionsRequest()
    if action == DESCRIBE_ACTION_OPTION:
        if len(rest) != 1:
            raise WorkflowUsageError(workflow_argument_option_usage(action))
        return DescribeActionRequest(rest[0])
    if action == DESCRIBE_PACKAGE_SCRIPT_OPTION:
        if len(rest) != 1:
            raise WorkflowUsageError(workflow_argument_option_usage(action))
        return DescribePackageScriptRequest(rest[0])
    return ExecuteActionRequest(action, rest)


__all__ = ["parse_workflow_args"]
