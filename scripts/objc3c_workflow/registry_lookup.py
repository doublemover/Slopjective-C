"""Action-catalog lookup helpers for workflow registry views."""

from __future__ import annotations

from .action_catalog import ACTION_SPECS
from .action_spec import ActionSpec


def catalog_action_spec(action: str) -> ActionSpec | None:
    return ACTION_SPECS.get(action)


def require_catalog_action_spec(action: str) -> ActionSpec:
    return ACTION_SPECS[action]


def catalog_has_action(action: str) -> bool:
    return action in ACTION_SPECS


__all__ = [
    "catalog_action_spec",
    "catalog_has_action",
    "require_catalog_action_spec",
]
