"""Section assembly helpers for the objc3c workflow action catalog."""

from __future__ import annotations

from collections.abc import Mapping

from .action_spec import ActionSpec


def merge_action_catalog_sections(
    *sections: Mapping[str, ActionSpec],
) -> dict[str, ActionSpec]:
    catalog: dict[str, ActionSpec] = {}
    duplicate_actions: list[str] = []
    for section in sections:
        for action, spec in section.items():
            if action in catalog:
                duplicate_actions.append(action)
                continue
            catalog[action] = spec
    if duplicate_actions:
        duplicates = ", ".join(sorted(duplicate_actions))
        raise ValueError(f"duplicate workflow action definitions: {duplicates}")
    return catalog
