"""Typed records for seed batch generation."""

from __future__ import annotations

from dataclasses import dataclass


class ParseError(ValueError):
    """Raised when the matrix cannot be parsed deterministically."""


@dataclass(frozen=True)
class SeedRow:
    seed_id: str
    family: str
    worklane: str
    proposed_issue_title: str
    artifact_targets: tuple[str, ...]
    depends_on: tuple[str, ...]
    shard_class: str
    acceptance_gate_id: str


@dataclass(frozen=True)
class EdgeRow:
    edge_id: str
    predecessor: str
    successor: str
    edge_type: str
    rationale: str


@dataclass(frozen=True)
class BatchRow:
    batch_id: str
    batch_class: str
    included_seed_ids: tuple[str, ...]
    entry_prerequisites: str
    exit_signal: str


@dataclass(frozen=True)
class PriorityRow:
    seed_id: str
    cpi: int
    duv: int
    rbv: int
    erc: int
    ecp: int
    dc: int
    priority_score: int
    tier: str


@dataclass(frozen=True)
class SeedOwnerAssignment:
    seed_id: str
    owner_primary: str
    owner_backup: str


@dataclass(frozen=True)
class OwnerMapContract:
    contract_id: str
    seed_id: str
    snapshot_date: str
    source_matrix_path: str
    owner_registry: dict[str, SeedOwnerAssignment]


@dataclass(frozen=True)
class SeedMatrix:
    snapshot_date: str
    seeds: list[SeedRow]
    edges: list[EdgeRow]
    waves: dict[str, tuple[str, ...]]
    batches: list[BatchRow]
    priorities: dict[str, PriorityRow]
