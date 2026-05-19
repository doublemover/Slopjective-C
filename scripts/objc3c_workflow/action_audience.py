"""Audience classification facade for public workflow action payloads."""

from __future__ import annotations

from .action_audience_constants import (
    ACTION_AUDIENCE_CONTRACT_ID,
    ACTION_AUDIENCE_OWNER_SURFACE,
    AUDIENCE_MAINTAINER,
    AUDIENCE_OPERATOR,
)
from .action_audience_prefixes import (
    MAINTAINER_PREFIXES,
    OPERATOR_EXACT_ACTIONS,
    OPERATOR_PREFIXES,
)
from .action_audience_rules import action_audience, action_audience_contract_payload


__all__ = [
    "ACTION_AUDIENCE_CONTRACT_ID",
    "ACTION_AUDIENCE_OWNER_SURFACE",
    "AUDIENCE_MAINTAINER",
    "AUDIENCE_OPERATOR",
    "MAINTAINER_PREFIXES",
    "OPERATOR_EXACT_ACTIONS",
    "OPERATOR_PREFIXES",
    "action_audience",
    "action_audience_contract_payload",
]
