"""Section assembly helpers for workflow action handlers."""

from __future__ import annotations

from collections.abc import Mapping

from scripts.objc3c_workflow.action_section_merge import merge_named_action_sections
from scripts.objc3c_workflow.action_spec import ActionHandler


def merge_action_handler_sections(
    *sections: Mapping[str, ActionHandler],
) -> dict[str, ActionHandler]:
    return merge_named_action_sections("handlers", *sections)


__all__ = ["merge_action_handler_sections"]
