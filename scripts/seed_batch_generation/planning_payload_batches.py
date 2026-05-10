"""Batch payload construction for seed planning output."""

from __future__ import annotations

from .config import CLASS_RANK
from .models import BatchRow, ParseError, PriorityRow, SeedOwnerAssignment
from .planning_payload_context import PlanningPayloadContext
from .planning_payload_issues import build_issue_template
from .planning_topology import parse_entry_dependencies
from .planning_topology import seed_sort_key


def build_batch_rows(
    *,
    batches: list[BatchRow],
    context: PlanningPayloadContext,
    priorities: dict[str, PriorityRow],
    owner_assignments: dict[str, SeedOwnerAssignment] | None,
) -> tuple[list[dict[str, object]], dict[str, list[str]]]:
    batch_rows = []
    for batch in batches:
        batch_rows.append(
            build_batch_row(
                batch=batch,
                context=context,
                priorities=priorities,
                owner_assignments=owner_assignments,
            )
        )

    batch_rows.sort(
        key=lambda row: (
            int(row["wave_span"]["min_wave_id"][1:]),
            CLASS_RANK.get(str(row["class"]), 99),
            str(row["batch_id"]),
        )
    )

    wave_to_batches: dict[str, list[str]] = {wave_id: [] for wave_id in context.wave_order}
    for batch in batch_rows:
        for wave_id in batch["wave_coverage"]:
            wave_to_batches[wave_id].append(str(batch["batch_id"]))
    return batch_rows, wave_to_batches


def build_batch_row(
    *,
    batch: BatchRow,
    context: PlanningPayloadContext,
    priorities: dict[str, PriorityRow],
    owner_assignments: dict[str, SeedOwnerAssignment] | None,
) -> dict[str, object]:
    for seed_id in batch.included_seed_ids:
        if seed_id not in context.seed_to_wave:
            raise ParseError(
                f"batch {batch.batch_id} seed id missing from wave table: {seed_id}"
            )

    wave_coverage = sorted(
        {context.seed_to_wave[seed_id] for seed_id in batch.included_seed_ids},
        key=lambda wave_id: int(wave_id[1:]),
    )
    ordered_seed_ids = sorted(
        batch.included_seed_ids,
        key=lambda seed_id: (
            context.wave_index_by_id[context.seed_to_wave[seed_id]],
            seed_sort_key(seed_id, priorities),
        ),
    )
    issue_templates = [
        build_issue_template(
            batch=batch,
            seed=context.seeds_by_id[seed_id],
            priority=priorities[seed_id],
            wave_id=context.seed_to_wave[seed_id],
            owner_assignments=owner_assignments,
        )
        for seed_id in ordered_seed_ids
    ]

    min_wave_index = min(context.wave_index_by_id[wave_id] for wave_id in wave_coverage)
    max_wave_index = max(context.wave_index_by_id[wave_id] for wave_id in wave_coverage)
    return {
        "batch_id": batch.batch_id,
        "class": batch.batch_class,
        "included_seed_ids": list(batch.included_seed_ids),
        "entry_prerequisites": batch.entry_prerequisites,
        "entry_dependency_ids": list(parse_entry_dependencies(batch.entry_prerequisites)),
        "exit_signal": batch.exit_signal,
        "wave_coverage": wave_coverage,
        "wave_span": {
            "min_wave_id": f"W{min_wave_index}",
            "max_wave_id": f"W{max_wave_index}",
        },
        "ordered_seed_ids": ordered_seed_ids,
        "issue_templates": issue_templates,
    }


__all__ = ["build_batch_row", "build_batch_rows"]
