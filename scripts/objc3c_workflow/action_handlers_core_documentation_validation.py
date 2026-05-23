"""Core documentation validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.docs_documentation import (
    VALIDATE_UMBRELLA_READINESS_ACTION,
    action_validate_umbrella_readiness,
)

CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    VALIDATE_UMBRELLA_READINESS_ACTION: action_validate_umbrella_readiness,
}

__all__ = ["CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS"]
