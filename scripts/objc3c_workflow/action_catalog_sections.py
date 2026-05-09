"""Section assembly helpers for the objc3c workflow action catalog."""

from __future__ import annotations

from collections.abc import Mapping

from .action_section_merge import merge_named_action_sections
from .action_spec import ActionSpec


def merge_action_catalog_sections(
    *sections: Mapping[str, ActionSpec],
) -> dict[str, ActionSpec]:
    return merge_named_action_sections("definitions", *sections)


__all__ = ["merge_action_catalog_sections"]
