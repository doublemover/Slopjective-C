"""Release-governance gate owner models."""

from __future__ import annotations

from dataclasses import dataclass

from ..release_governance_owner_models import ReleaseGateOwner


@dataclass(frozen=True)
class ReleaseGateLookup:
    gate_id: str


ReleaseGateOwnerCatalog = dict[str, ReleaseGateOwner]

__all__ = [
    "ReleaseGateLookup",
    "ReleaseGateOwner",
    "ReleaseGateOwnerCatalog",
]
