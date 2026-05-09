"""Shared action-section merge helpers."""

from __future__ import annotations

from collections.abc import Mapping
from dataclasses import dataclass
from typing import TypeVar

ActionSectionValue = TypeVar("ActionSectionValue")


@dataclass(frozen=True)
class DuplicateActionSection:
    action: str
    first_section_index: int
    duplicate_section_index: int


def duplicate_action_names(duplicates: list[DuplicateActionSection]) -> list[str]:
    return sorted({duplicate.action for duplicate in duplicates})


def duplicate_action_message(
    kind: str,
    duplicates: list[DuplicateActionSection],
) -> str:
    duplicate_names = ", ".join(duplicate_action_names(duplicates))
    return f"duplicate workflow action {kind}: {duplicate_names}"


def merge_named_action_sections(
    kind: str,
    *sections: Mapping[str, ActionSectionValue],
) -> dict[str, ActionSectionValue]:
    merged: dict[str, ActionSectionValue] = {}
    owner_sections: dict[str, int] = {}
    duplicate_actions: list[DuplicateActionSection] = []
    for section_index, section in enumerate(sections):
        for action, value in section.items():
            if action in merged:
                duplicate_actions.append(
                    DuplicateActionSection(
                        action=action,
                        first_section_index=owner_sections[action],
                        duplicate_section_index=section_index,
                    )
                )
                continue
            merged[action] = value
            owner_sections[action] = section_index
    if duplicate_actions:
        raise ValueError(duplicate_action_message(kind, duplicate_actions))
    return merged


__all__ = [
    "DuplicateActionSection",
    "duplicate_action_message",
    "duplicate_action_names",
    "merge_named_action_sections",
]
