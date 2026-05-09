"""Action handler registry for the objc3c workflow CLI."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_groups import ACTION_HANDLER_SECTION_GROUPS
from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_spec import ActionHandler

ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    *ACTION_HANDLER_SECTION_GROUPS
)


__all__ = ["ACTION_HANDLERS"]
