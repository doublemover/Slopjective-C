"""Audience classification rules for public workflow actions."""

from __future__ import annotations

from .action_audience_constants import AUDIENCE_MAINTAINER, AUDIENCE_OPERATOR
from .action_audience_prefixes import MAINTAINER_PREFIXES, OPERATOR_PREFIXES


def action_audience(action: str) -> str:
    if action.startswith("build-native") or action == "build-default":
        return AUDIENCE_OPERATOR
    if action.startswith(MAINTAINER_PREFIXES):
        return AUDIENCE_MAINTAINER
    if action.startswith(OPERATOR_PREFIXES):
        return AUDIENCE_OPERATOR
    return AUDIENCE_OPERATOR


__all__ = ["action_audience"]
