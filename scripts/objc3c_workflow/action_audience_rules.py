"""Audience classification rules for public workflow actions."""

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


def action_audience(action: str) -> str:
    if action in OPERATOR_EXACT_ACTIONS:
        return AUDIENCE_OPERATOR
    if action.startswith(OPERATOR_PREFIXES):
        return AUDIENCE_OPERATOR
    if action.startswith(MAINTAINER_PREFIXES):
        return AUDIENCE_MAINTAINER
    raise ValueError(f"unclassified workflow action audience: {action}")


def action_audience_contract_payload() -> dict[str, object]:
    return {
        "contract_id": ACTION_AUDIENCE_CONTRACT_ID,
        "owner_surface": ACTION_AUDIENCE_OWNER_SURFACE,
        "operator_exact_actions": list(OPERATOR_EXACT_ACTIONS),
        "operator_prefixes": list(OPERATOR_PREFIXES),
        "maintainer_prefixes": list(MAINTAINER_PREFIXES),
        "hidden_internal_public_split_allowed": False,
        "unknown_audience_retired_route_allowed": False,
        "public_command_aliases_allowed": False,
        "public_contract": True,
    }


__all__ = ["action_audience", "action_audience_contract_payload"]
