"""Action-catalog iteration helpers for workflow registry views."""

from __future__ import annotations

from .action_catalog import ACTION_SPECS
from .action_spec import ActionSpec


def catalog_action_names() -> list[str]:
    return list(ACTION_SPECS)


def catalog_action_specs() -> list[ActionSpec]:
    return list(ACTION_SPECS.values())


def catalog_action_items() -> list[tuple[str, ActionSpec]]:
    return list(ACTION_SPECS.items())


__all__ = [
    "catalog_action_items",
    "catalog_action_names",
    "catalog_action_specs",
]
