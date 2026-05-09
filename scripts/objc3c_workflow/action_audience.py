"""Audience classification facade for public workflow action payloads."""

from __future__ import annotations

from .action_audience_constants import AUDIENCE_MAINTAINER, AUDIENCE_OPERATOR
from .action_audience_prefixes import MAINTAINER_PREFIXES, OPERATOR_PREFIXES
from .action_audience_rules import action_audience


__all__ = [
    "AUDIENCE_MAINTAINER",
    "AUDIENCE_OPERATOR",
    "MAINTAINER_PREFIXES",
    "OPERATOR_PREFIXES",
    "action_audience",
]
