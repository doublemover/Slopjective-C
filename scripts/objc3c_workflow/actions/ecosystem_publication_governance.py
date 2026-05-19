"""Governance-sustainability publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_contracts import (
    GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY,
    GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY,
)
from .ecosystem_publication_runner import run_publication_action


def action_validate_governance_sustainability(_: list[str]) -> int:
    return run_publication_action("validate-governance-sustainability")


def action_publish_governance_sustainability(_: list[str]) -> int:
    return run_publication_action("publish-governance-sustainability")


__all__ = [
    "GOVERNANCE_SUSTAINABILITY_INTEGRATION_PY",
    "GOVERNANCE_SUSTAINABILITY_PUBLICATION_PY",
    "action_publish_governance_sustainability",
    "action_validate_governance_sustainability",
]
