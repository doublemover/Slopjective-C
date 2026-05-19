"""Public workflow command construction."""

from __future__ import annotations

from .public_command_constants import WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS


def public_workflow_command(*args: str) -> list[str]:
    return [*WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS, *args]


def public_workflow_command_tuple(*args: str) -> tuple[str, ...]:
    return tuple(public_workflow_command(*args))


def public_workflow_list_command() -> list[str]:
    return public_workflow_command("--list-json")


def public_workflow_describe_command(action: str) -> list[str]:
    return public_workflow_command("--describe", action)


__all__ = [
    "public_workflow_command",
    "public_workflow_command_tuple",
    "public_workflow_describe_command",
    "public_workflow_list_command",
]
