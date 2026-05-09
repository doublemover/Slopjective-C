"""Canonical objc3c workflow action catalog."""

from __future__ import annotations

from .action_catalog_groups import ACTION_CATALOG_SECTION_GROUPS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    *ACTION_CATALOG_SECTION_GROUPS
)


__all__ = ["ACTION_SPECS"]
