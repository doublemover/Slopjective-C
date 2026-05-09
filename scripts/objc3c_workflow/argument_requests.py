"""Typed request models for the objc3c workflow CLI."""

from __future__ import annotations

from dataclasses import dataclass


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


__all__ = [
    "DescribeActionRequest",
    "DescribePackageScriptRequest",
    "ExecuteActionRequest",
    "ListActionsRequest",
    "WorkflowRequest",
    "WorkflowUsageError",
]
