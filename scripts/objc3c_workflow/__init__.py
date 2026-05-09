"""Canonical objc3c workflow package."""

from __future__ import annotations

from .action_spec import ActionSpec
from .action_catalog import ACTION_SPECS

__all__ = ["ACTION_SPECS", "ActionSpec"]
