"""Section assembly helpers for workflow action handlers."""

from __future__ import annotations

from collections.abc import Mapping

from scripts.objc3c_workflow.action_spec import ActionHandler


def merge_action_handler_sections(
    *sections: Mapping[str, ActionHandler],
) -> dict[str, ActionHandler]:
    handlers: dict[str, ActionHandler] = {}
    duplicate_actions: list[str] = []
    for section in sections:
        for action, handler in section.items():
            if action in handlers:
                duplicate_actions.append(action)
                continue
            handlers[action] = handler
    if duplicate_actions:
        duplicates = ", ".join(sorted(duplicate_actions))
        raise ValueError(f"duplicate workflow action handlers: {duplicates}")
    return handlers
