"""Tooling hygiene handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import hygiene

TOOLING_HYGIENE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "lint-spec": hygiene.action_lint_spec,
}

__all__ = ["TOOLING_HYGIENE_ACTION_HANDLERS"]
