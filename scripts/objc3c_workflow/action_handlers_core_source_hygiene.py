"""Core source-hygiene handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import hygiene

CORE_SOURCE_HYGIENE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-source-hygiene-authenticity": hygiene.action_check_source_hygiene_authenticity,
    "check-source-hygiene-hard-cutover": hygiene.action_check_source_hygiene_hard_cutover,
    "check-task-hygiene": hygiene.action_check_task_hygiene,
}

__all__ = ["CORE_SOURCE_HYGIENE_ACTION_HANDLERS"]
