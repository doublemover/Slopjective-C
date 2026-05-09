"""Core markdown handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_MARKDOWN_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-markdown": docs.action_check_markdown,
    "format-markdown": docs.action_format_markdown,
    "lint-markdown": docs.action_lint_markdown,
}

__all__ = ["CORE_MARKDOWN_ACTION_HANDLERS"]
