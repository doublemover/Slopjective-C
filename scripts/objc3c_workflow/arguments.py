"""Argument parsing for the objc3c workflow CLI."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

from .environment import WORKFLOW_COMMAND_TEXT


@dataclass(frozen=True)
class WorkflowUsageError(Exception):
    message: str
    exit_code: int = 2


@dataclass(frozen=True)
class ListActionsRequest:
    pass


@dataclass(frozen=True)
class DescribeActionRequest:
    action: str


@dataclass(frozen=True)
class DescribePackageScriptRequest:
    package_script: str


@dataclass(frozen=True)
class ExecuteActionRequest:
    action: str
    args: list[str]


WorkflowRequest = (
    ListActionsRequest
    | DescribeActionRequest
    | DescribePackageScriptRequest
    | ExecuteActionRequest
)


def usage_text() -> str:
    return (
        f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]\n"
        f"       {WORKFLOW_COMMAND_TEXT} --list-json\n"
        f"       {WORKFLOW_COMMAND_TEXT} --describe <action>\n"
        f"       {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>"
    )


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
