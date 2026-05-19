"""In-process nested action execution for composite workflow steps."""

from __future__ import annotations

from collections.abc import Sequence
from pathlib import Path

from .composite_step_payload import composite_step_payload
from .public_bridge_constants import WORKFLOW_BRIDGE_SCRIPT

NPM_EXECUTABLE_NAMES = frozenset({"npm", "npm.cmd", "npm.exe"})
NPM_WORKFLOW_COMMAND_PREFIX = ("run", WORKFLOW_BRIDGE_SCRIPT, "--")
NPM_WORKFLOW_ACTION_OFFSET = 4


def npm_bridge_action_offset(normalized: Sequence[str]) -> int | None:
    if len(normalized) < NPM_WORKFLOW_ACTION_OFFSET + 1:
        return None
    executable_name = Path(normalized[0]).name.lower()
    if executable_name not in NPM_EXECUTABLE_NAMES:
        return None
    if tuple(normalized[1:NPM_WORKFLOW_ACTION_OFFSET]) != NPM_WORKFLOW_COMMAND_PREFIX:
        return None
    return NPM_WORKFLOW_ACTION_OFFSET


def execute_nested_action(action: str, rest: list[str]) -> int:
    from .action_execution_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


def in_process_nested_step(
    action: str,
    normalized: list[str],
    started_at: float,
) -> dict[str, object] | None:
    action_offset = npm_bridge_action_offset(normalized)
    if action_offset is not None:
        nested_action = normalized[action_offset]
        nested_rest = normalized[action_offset + 1 :]
        exit_code = execute_nested_action(nested_action, nested_rest)
        return composite_step_payload(
            action=action,
            command=normalized,
            exit_code=exit_code,
            report_paths=[],
            started_at=started_at,
            extra={"executed_in_process": True},
        )
    return None
