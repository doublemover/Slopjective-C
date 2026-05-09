"""Shared action-section merge helpers."""

from __future__ import annotations

from collections.abc import Mapping
from typing import TypeVar

ActionSectionValue = TypeVar("ActionSectionValue")


def duplicate_action_message(kind: str, actions: list[str]) -> str:
    duplicates = ", ".join(sorted(actions))
    return f"duplicate workflow action {kind}: {duplicates}"


def merge_named_action_sections(
    kind: str,
    *sections: Mapping[str, ActionSectionValue],
) -> dict[str, ActionSectionValue]:
    merged: dict[str, ActionSectionValue] = {}
    duplicate_actions: list[str] = []
    for section in sections:
        for action, value in section.items():
            if action in merged:
                duplicate_actions.append(action)
                continue
            merged[action] = value
    if duplicate_actions:
        raise ValueError(duplicate_action_message(kind, duplicate_actions))
    return merged


__all__ = ["duplicate_action_message", "merge_named_action_sections"]
