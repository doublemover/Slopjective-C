"""Seed batch graph validation helpers."""

from __future__ import annotations

from .config import REQUIRED_EDGE_IDS, REQUIRED_WAVE_IDS
from .models import BatchRow, EdgeRow, ParseError, PriorityRow, SeedRow


def validate_required_edges(edges: list[EdgeRow]) -> None:
    edge_ids = sorted(edge.edge_id for edge in edges)
    if edge_ids != REQUIRED_EDGE_IDS:
        missing = sorted(set(REQUIRED_EDGE_IDS) - set(edge_ids))
        extra = sorted(set(edge_ids) - set(REQUIRED_EDGE_IDS))
        raise ParseError(
            "edge id set mismatch; "
            f"missing={missing if missing else 'none'}, "
            f"extra={extra if extra else 'none'}"
        )


def validate_required_waves(waves: dict[str, tuple[str, ...]]) -> None:
    wave_ids = sorted(waves.keys(), key=lambda value: int(value[1:]))
    if wave_ids != REQUIRED_WAVE_IDS:
        missing = sorted(set(REQUIRED_WAVE_IDS) - set(wave_ids))
        extra = sorted(set(wave_ids) - set(REQUIRED_WAVE_IDS))
        raise ParseError(
            "wave id set mismatch; "
            f"missing={missing if missing else 'none'}, "
            f"extra={extra if extra else 'none'}"
        )


def ensure_seed_references_exist(
    *,
    seeds: dict[str, SeedRow],
    edges: list[EdgeRow],
    waves: dict[str, tuple[str, ...]],
    batches: list[BatchRow],
    priorities: dict[str, PriorityRow],
) -> None:
    for edge in edges:
        if edge.predecessor not in seeds:
            raise ParseError(
                f"edge {edge.edge_id} predecessor not found in seed table: {edge.predecessor}"
            )
        if edge.successor not in seeds:
            raise ParseError(
                f"edge {edge.edge_id} successor not found in seed table: {edge.successor}"
            )

    for seed in seeds.values():
        for dependency in seed.depends_on:
            if dependency not in seeds:
                raise ParseError(
                    f"seed {seed.seed_id} depends on unknown seed id: {dependency}"
                )

    for wave_id, seed_ids in waves.items():
        for seed_id in seed_ids:
            if seed_id not in seeds:
                raise ParseError(f"wave {wave_id} references unknown seed id: {seed_id}")

    for batch in batches:
        for seed_id in batch.included_seed_ids:
            if seed_id not in seeds:
                raise ParseError(
                    f"batch {batch.batch_id} references unknown seed id: {seed_id}"
                )

    missing_priority = sorted(set(seeds.keys()) - set(priorities.keys()))
    if missing_priority:
        raise ParseError(
            "priority table is missing seed ids: " + ", ".join(missing_priority)
        )


def validate_seed_dependencies_against_edges(
    *,
    seeds: dict[str, SeedRow],
    edges: list[EdgeRow],
) -> None:
    edge_dependencies_by_successor: dict[str, set[str]] = {
        seed_id: set() for seed_id in seeds
    }
    seen_pairs: dict[tuple[str, str], str] = {}
    for edge in edges:
        pair = (edge.predecessor, edge.successor)
        prior_edge_id = seen_pairs.get(pair)
        if prior_edge_id is not None:
            raise ParseError(
                "duplicate dependency edge pair in dependency table: "
                f"{edge.predecessor} -> {edge.successor} "
                f"(edge ids: {prior_edge_id}, {edge.edge_id})"
            )
        seen_pairs[pair] = edge.edge_id
        edge_dependencies_by_successor[edge.successor].add(edge.predecessor)

    for seed_id, seed in sorted(seeds.items()):
        documented_dependencies = set(seed.depends_on)
        edge_dependencies = edge_dependencies_by_successor[seed_id]
        if documented_dependencies != edge_dependencies:
            missing_edges = sorted(documented_dependencies - edge_dependencies)
            extra_edges = sorted(edge_dependencies - documented_dependencies)
            raise ParseError(
                f"seed {seed_id} dependency mismatch between seed table and edge table; "
                f"missing_edges={missing_edges if missing_edges else 'none'}, "
                f"extra_edges={extra_edges if extra_edges else 'none'}"
            )


__all__ = [
    "ensure_seed_references_exist",
    "validate_required_edges",
    "validate_required_waves",
    "validate_seed_dependencies_against_edges",
]
