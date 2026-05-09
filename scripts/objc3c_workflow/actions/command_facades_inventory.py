"""Shared action-name inventory helpers for public command facades."""

from __future__ import annotations

from collections.abc import Callable

from ..action_spec import ActionSpec
from ..registry_views import actions_by_category, actions_matching


def category_action_names(category: str) -> list[str]:
    return actions_by_category(category)


def category_group_action_names(categories: tuple[str, ...]) -> list[str]:
    names: list[str] = []
    for category in categories:
        names.extend(actions_by_category(category))
    return names


def matching_action_names(predicate: Callable[[str, ActionSpec], bool]) -> list[str]:
    return actions_matching(predicate)
