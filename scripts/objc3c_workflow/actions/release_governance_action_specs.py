"""Release-governance action spec construction."""

from __future__ import annotations

from ..action_spec import ActionSpec
from .release_governance_owner_models import ReleaseGovernanceActionContract


def release_action_specs(
    contracts: tuple[ReleaseGovernanceActionContract, ...],
) -> dict[str, ActionSpec]:
    return {contract.action: contract.to_action_spec() for contract in contracts}


__all__ = ["release_action_specs"]
