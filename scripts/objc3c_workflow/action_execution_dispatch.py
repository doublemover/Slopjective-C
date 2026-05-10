"""Public registered-action execution dispatch."""

from __future__ import annotations

from scripts.objc3c_workflow.command_result_output import emit_result_error


def execute_registered_action(action: str, rest: list[str]) -> int:
    from scripts.objc3c_workflow.action_execution import (
        execute_registered_action_with_metadata,
    )

    result = execute_registered_action_with_metadata(action, rest)
    if not result.accepted:
        emit_result_error(result)
    return result.exit_code


__all__ = ["execute_registered_action"]
