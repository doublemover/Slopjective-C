"""Deterministic seed batch payload assembly."""

from __future__ import annotations

from pathlib import Path

from .models import BatchRow, EdgeRow, PriorityRow, SeedOwnerAssignment, SeedRow
from .planning_payload_batches import build_batch_rows
from .planning_payload_context import build_payload_context
from .planning_payload_graph import build_graph_payload


def build_payload(
    *,
    matrix_path: Path,
    snapshot_date: str,
    seeds: list[SeedRow],
    edges: list[EdgeRow],
    waves: dict[str, tuple[str, ...]],
    batches: list[BatchRow],
    priorities: dict[str, PriorityRow],
    owner_assignments: dict[str, SeedOwnerAssignment] | None,
) -> dict[str, object]:
    context = build_payload_context(
        seeds=seeds,
        edges=edges,
        waves=waves,
        batches=batches,
        priorities=priorities,
    )
    batch_rows, wave_to_batches = build_batch_rows(
        batches=batches,
        context=context,
        priorities=priorities,
        owner_assignments=owner_assignments,
    )
    return build_graph_payload(
        matrix_path=matrix_path,
        snapshot_date=snapshot_date,
        context=context,
        priorities=priorities,
        batch_rows=batch_rows,
        wave_to_batches=wave_to_batches,
    )


__all__ = ["build_payload"]
