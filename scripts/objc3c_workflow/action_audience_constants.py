"""Canonical audience labels for public workflow action payloads."""

from __future__ import annotations

AUDIENCE_MAINTAINER = "maintainer"
AUDIENCE_OPERATOR = "operator"
ACTION_AUDIENCE_CONTRACT_ID = "objc3c-workflow-action-audience-v1"
ACTION_AUDIENCE_OWNER_SURFACE = "scripts/objc3c_workflow/action_audience_rules.py"


__all__ = [
    "ACTION_AUDIENCE_CONTRACT_ID",
    "ACTION_AUDIENCE_OWNER_SURFACE",
    "AUDIENCE_MAINTAINER",
    "AUDIENCE_OPERATOR",
]
