"""Audience classification rules for public workflow actions."""

from __future__ import annotations

from .action_audience_prefixes import MAINTAINER_PREFIXES, OPERATOR_PREFIXES


def action_audience(action: str) -> str:
    if action.startswith("build-native") or action == "build-default":
        return "operator"
    if action.startswith(MAINTAINER_PREFIXES):
        return "maintainer"
    if action.startswith(OPERATOR_PREFIXES):
        return "operator"
    return "operator"


__all__ = ["action_audience"]
