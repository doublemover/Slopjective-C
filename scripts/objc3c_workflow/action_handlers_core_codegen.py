"""Core codegen optimization policy action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.codegen_optimization_policy import (
    CODEGEN_OPTIMIZATION_POLICY_ACTION,
    action_validate_codegen_optimization_policy,
)

CORE_CODEGEN_ACTION_HANDLERS: dict[str, ActionHandler] = {
    CODEGEN_OPTIMIZATION_POLICY_ACTION: action_validate_codegen_optimization_policy,
}

__all__ = ["CORE_CODEGEN_ACTION_HANDLERS"]
