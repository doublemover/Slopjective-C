from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class EvidenceOwner:
    owner_id: str
    source_contract: str
    summary_implementation_anchor: str
    check_implementation_anchors: tuple[str, ...] = ()
    supporting_contracts: tuple[str, ...] = ()


@dataclass(frozen=True)
class EvidenceFamily:
    boundary_inventory: str
    owners: tuple[EvidenceOwner, ...]
