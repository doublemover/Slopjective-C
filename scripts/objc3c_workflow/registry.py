"""Compatibility facade for the objc3c workflow action registry."""

from __future__ import annotations

from .action_catalog import ACTION_SPECS
from .registry_views import action_names, action_spec, actions_by_category

__all__ = [
    "ACTION_SPECS",
    "action_names",
    "action_spec",
    "actions_by_category",
]
