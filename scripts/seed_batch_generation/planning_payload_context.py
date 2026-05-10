"""Validated ordering context for seed batch payload assembly."""

from __future__ import annotations

from dataclasses import dataclass

from .models import BatchRow, EdgeRow, ParseError, PriorityRow, SeedRow
from .planning_topology import compute_topological_waves
from .planning_topology import seed_sort_key
from .planning_topology import validate_waves_against_topology
from .planning_validation import ensure_seed_references_exist
from .planning_validation import validate_required_edges
from .planning_validation import validate_required_waves
from .planning_validation import validate_seed_dependencies_against_edges


@dataclass(frozen=True)
class PlanningPayloadContext:
    seeds_by_id: dict[str, SeedRow]
    edge_rows: list[EdgeRow]
    seed_rows: list[SeedRow]
    wave_order: list[str]
    wave_index_by_id: dict[str, int]
    seed_to_wave: dict[str, str]
    wave_payload: list[dict[str, object]]
    execution_order: list[str]


def build_payload_context(
    *,
    seeds: list[SeedRow],
    edges: list[EdgeRow],
    waves: dict[str, tuple[str, ...]],
    batches: list[BatchRow],
    priorities: dict[str, PriorityRow],
) -> PlanningPayloadContext:
    seeds_by_id = {seed.seed_id: seed for seed in seeds}

    ensure_seed_references_exist(
        seeds=seeds_by_id,
        edges=edges,
        waves=waves,
        batches=batches,
        priorities=priorities,
    )
    validate_seed_dependencies_against_edges(seeds=seeds_by_id, edges=edges)
    validate_required_edges(edges)
    validate_required_waves(waves)
    validate_waves_against_topology(waves, compute_topological_waves(seeds_by_id, edges))

    wave_order = sorted(waves.keys(), key=lambda wave_id: int(wave_id[1:]))
    wave_index_by_id = {wave_id: index for index, wave_id in enumerate(wave_order)}
    seed_to_wave = build_seed_to_wave_map(seeds_by_id=seeds_by_id, waves=waves, wave_order=wave_order)
    wave_payload, execution_order = build_wave_payload(
        waves=waves,
        wave_order=wave_order,
        priorities=priorities,
    )

    return PlanningPayloadContext(
        seeds_by_id=seeds_by_id,
        edge_rows=sorted(edges, key=lambda row: row.edge_id),
        seed_rows=sorted(
            seeds,
            key=lambda row: (
                wave_index_by_id[seed_to_wave[row.seed_id]],
                seed_sort_key(row.seed_id, priorities),
            ),
        ),
        wave_order=wave_order,
        wave_index_by_id=wave_index_by_id,
        seed_to_wave=seed_to_wave,
        wave_payload=wave_payload,
        execution_order=execution_order,
    )


def build_seed_to_wave_map(
    *,
    seeds_by_id: dict[str, SeedRow],
    waves: dict[str, tuple[str, ...]],
    wave_order: list[str],
) -> dict[str, str]:
    seed_to_wave: dict[str, str] = {}
    for wave_id in wave_order:
        for seed_id in waves[wave_id]:
            if seed_id in seed_to_wave:
                raise ParseError(f"seed {seed_id} appears in multiple wave rows")
            seed_to_wave[seed_id] = wave_id

    missing_wave = sorted(set(seeds_by_id.keys()) - set(seed_to_wave.keys()))
    if missing_wave:
        raise ParseError(
            "seed ids missing from wave table: " + ", ".join(missing_wave)
        )
    return seed_to_wave


def build_wave_payload(
    *,
    waves: dict[str, tuple[str, ...]],
    wave_order: list[str],
    priorities: dict[str, PriorityRow],
) -> tuple[list[dict[str, object]], list[str]]:
    wave_payload: list[dict[str, object]] = []
    execution_order: list[str] = []
    for wave_id in wave_order:
        documented_seed_ids = list(waves[wave_id])
        ordered_seed_ids = sorted(
            documented_seed_ids,
            key=lambda seed_id: seed_sort_key(seed_id, priorities),
        )
        execution_order.extend(ordered_seed_ids)
        wave_payload.append(
            {
                "wave_id": wave_id,
                "eligible_seed_ids": documented_seed_ids,
                "ordered_seed_ids": ordered_seed_ids,
            }
        )
    return wave_payload, execution_order


__all__ = [
    "PlanningPayloadContext",
    "build_payload_context",
    "build_seed_to_wave_map",
    "build_wave_payload",
]
