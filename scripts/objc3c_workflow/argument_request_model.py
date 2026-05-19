"""Typed request models for parsed workflow CLI arguments."""

from __future__ import annotations

from dataclasses import dataclass


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
]
