"""Final graph payload assembly for seed batch planning."""

from __future__ import annotations

from pathlib import Path

from .config import REQUIRED_EDGE_IDS, REQUIRED_WAVE_IDS, ROOT
from .models import PriorityRow
from .planning_payload_context import PlanningPayloadContext


def build_graph_payload(
    *,
    matrix_path: Path,
    snapshot_date: str,
    context: PlanningPayloadContext,
    priorities: dict[str, PriorityRow],
    batch_rows: list[dict[str, object]],
    wave_to_batches: dict[str, list[str]],
) -> dict[str, object]:
    return {
        "contract_id": "V013-TOOL-03-SEED-DAG-v1",
        "seed_id": "V013-TOOL-03",
        "snapshot_date": snapshot_date,
        "source_matrix_path": matrix_path.relative_to(ROOT).as_posix(),
        "determinism": {
            "ordering_rules": [
                "Within each documented wave, order seeds by priority score descending.",
                "Tie-break seed ordering by DUV descending, then DC ascending, then Seed ID ascending.",
                "Order batches by min covered wave ascending, then class rank (small<medium<large), then Batch ID ascending.",
            ],
            "required_edge_ids": REQUIRED_EDGE_IDS,
            "required_wave_ids": REQUIRED_WAVE_IDS,
            "cycle_check": "pass",
            "documented_wave_topology_check": "pass",
        },
        "graph": build_seed_graph(context=context, priorities=priorities),
        "wave_eligibility": context.wave_payload,
        "execution_order": context.execution_order,
        "batch_skeletons": {
            "batch_count": len(batch_rows),
            "batches": batch_rows,
            "wave_to_batches": [
                {
                    "wave_id": wave_id,
                    "batch_ids": wave_to_batches[wave_id],
                }
                for wave_id in context.wave_order
            ],
        },
    }


def build_seed_graph(
    *,
    context: PlanningPayloadContext,
    priorities: dict[str, PriorityRow],
) -> dict[str, object]:
    return {
        "seed_count": len(context.seed_rows),
        "edge_count": len(context.edge_rows),
        "seeds": [
            {
                "seed_id": row.seed_id,
                "family": row.family,
                "worklane": row.worklane,
                "proposed_issue_title": row.proposed_issue_title,
                "artifact_targets": list(row.artifact_targets),
                "depends_on": list(row.depends_on),
                "shard_class": row.shard_class,
                "acceptance_gate_id": row.acceptance_gate_id,
                "priority": {
                    "cpi": priorities[row.seed_id].cpi,
                    "duv": priorities[row.seed_id].duv,
                    "rbv": priorities[row.seed_id].rbv,
                    "erc": priorities[row.seed_id].erc,
                    "ecp": priorities[row.seed_id].ecp,
                    "dc": priorities[row.seed_id].dc,
                    "priority_score": priorities[row.seed_id].priority_score,
                    "tier": priorities[row.seed_id].tier,
                },
                "wave_id": context.seed_to_wave[row.seed_id],
            }
            for row in context.seed_rows
        ],
        "edges": [
            {
                "edge_id": row.edge_id,
                "predecessor": row.predecessor,
                "successor": row.successor,
                "type": row.edge_type,
                "rationale": row.rationale,
            }
            for row in context.edge_rows
        ],
    }


__all__ = ["build_graph_payload", "build_seed_graph"]
