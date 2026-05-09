"""Core public command-surface handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.docs_public_commands import (
    BUILD_PUBLIC_COMMAND_CONTRACT_ACTION,
    BUILD_PUBLIC_COMMAND_SURFACE_ACTION,
    CHECK_PUBLIC_COMMAND_BUDGET_ACTION,
    CHECK_PUBLIC_COMMAND_CONTRACT_ACTION,
    CHECK_PUBLIC_COMMAND_SURFACE_ACTION,
    action_build_public_command_contract,
    action_build_public_command_surface,
    action_check_public_command_budget,
    action_check_public_command_contract,
    action_check_public_command_surface,
)

CORE_PUBLIC_COMMAND_ACTION_HANDLERS: dict[str, ActionHandler] = {
    BUILD_PUBLIC_COMMAND_SURFACE_ACTION: action_build_public_command_surface,
    CHECK_PUBLIC_COMMAND_SURFACE_ACTION: action_check_public_command_surface,
    BUILD_PUBLIC_COMMAND_CONTRACT_ACTION: action_build_public_command_contract,
    CHECK_PUBLIC_COMMAND_CONTRACT_ACTION: action_check_public_command_contract,
    CHECK_PUBLIC_COMMAND_BUDGET_ACTION: action_check_public_command_budget,
}

__all__ = ["CORE_PUBLIC_COMMAND_ACTION_HANDLERS"]
