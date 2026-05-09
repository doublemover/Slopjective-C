"""Core public command-surface handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_PUBLIC_COMMAND_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-public-command-surface": docs.action_build_public_command_surface,
    "check-public-command-surface": docs.action_check_public_command_surface,
    "build-public-command-contract": docs.action_build_public_command_contract,
    "check-public-command-contract": docs.action_check_public_command_contract,
    "check-public-command-budget": docs.action_check_public_command_budget,
}

__all__ = ["CORE_PUBLIC_COMMAND_ACTION_HANDLERS"]
