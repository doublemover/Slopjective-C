"""Graph validation and deterministic output planning."""

from __future__ import annotations

from pathlib import Path

from .config import (
    CLASS_RANK,
    DEFAULT_UNASSIGNED_OWNER,
    FAMILY_LABEL,
    REQUIRED_EDGE_IDS,
    REQUIRED_WAVE_IDS,
    ROOT,
    TOKEN_RE,
    WORKLANE_LABEL,
)
from .models import BatchRow, EdgeRow, ParseError, PriorityRow, SeedOwnerAssignment, SeedRow


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


def seed_sort_key(
    seed_id: str,
    priorities: dict[str, PriorityRow],
) -> tuple[int, int, int, str]:
    row = priorities.get(seed_id)
    if row is None:
        return (0, 0, 0, seed_id)
    return (-row.priority_score, -row.duv, row.dc, seed_id)


def compute_topological_waves(
    seeds: dict[str, SeedRow],
    edges: list[EdgeRow],
) -> dict[str, list[str]]:
    adjacency: dict[str, set[str]] = {seed_id: set() for seed_id in seeds}
    indegree: dict[str, int] = {seed_id: 0 for seed_id in seeds}

    for edge in edges:
        if edge.predecessor not in seeds:
            raise ParseError(
                f"edge {edge.edge_id} references unknown predecessor {edge.predecessor}"
            )
        if edge.successor not in seeds:
            raise ParseError(
                f"edge {edge.edge_id} references unknown successor {edge.successor}"
            )
        if edge.successor not in adjacency[edge.predecessor]:
            adjacency[edge.predecessor].add(edge.successor)
            indegree[edge.successor] += 1

    current = sorted(seed_id for seed_id, count in indegree.items() if count == 0)
    remaining = dict(indegree)
    computed: dict[str, list[str]] = {}
    wave_index = 0
    processed = 0

    while current:
        wave_id = f"W{wave_index}"
        computed[wave_id] = current
        processed += len(current)
        next_ready: set[str] = set()

        for seed_id in current:
            for successor in adjacency[seed_id]:
                remaining[successor] -= 1
                if remaining[successor] == 0:
                    next_ready.add(successor)

        current = sorted(next_ready)
        wave_index += 1

    if processed != len(seeds):
        unresolved = sorted(
            seed_id for seed_id, count in remaining.items() if count > 0
        )
        raise ParseError(f"dependency graph contains a cycle; unresolved nodes: {unresolved}")

    return computed


def normalize_wave_membership(
    waves: dict[str, tuple[str, ...]],
) -> dict[str, set[str]]:
    normalized: dict[str, set[str]] = {}
    for wave_id, seed_ids in waves.items():
        normalized[wave_id] = set(seed_ids)
    return normalized


def validate_waves_against_topology(
    documented_waves: dict[str, tuple[str, ...]],
    computed_waves: dict[str, list[str]],
) -> None:
    documented_membership = normalize_wave_membership(documented_waves)
    computed_membership = normalize_wave_membership(
        {wave_id: tuple(seed_ids) for wave_id, seed_ids in computed_waves.items()}
    )
    if documented_membership != computed_membership:
        raise ParseError(
            "documented wave memberships do not match computed topological wave memberships"
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


def parse_entry_dependencies(entry_prerequisites: str) -> tuple[str, ...]:
    tokens = TOKEN_RE.findall(entry_prerequisites)
    unique_tokens: list[str] = []
    for token in tokens:
        if token not in unique_tokens:
            unique_tokens.append(token)
    return tuple(unique_tokens)


def split_seed_title_action(proposed_issue_title: str) -> str:
    trimmed = proposed_issue_title.strip()
    if "] " in trimmed:
        return trimmed.split("] ", 1)[1]
    return trimmed


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

    computed_waves = compute_topological_waves(seeds_by_id, edges)
    validate_waves_against_topology(waves, computed_waves)

    wave_order = sorted(waves.keys(), key=lambda wave_id: int(wave_id[1:]))
    wave_index_by_id = {wave_id: index for index, wave_id in enumerate(wave_order)}

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

    edge_rows = sorted(edges, key=lambda row: row.edge_id)
    seed_rows = sorted(
        seeds,
        key=lambda row: (wave_index_by_id[seed_to_wave[row.seed_id]], seed_sort_key(row.seed_id, priorities)),
    )

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

    batch_rows = []
    for batch in batches:
        for seed_id in batch.included_seed_ids:
            if seed_id not in seed_to_wave:
                raise ParseError(
                    f"batch {batch.batch_id} seed id missing from wave table: {seed_id}"
                )

        wave_coverage = sorted(
            {seed_to_wave[seed_id] for seed_id in batch.included_seed_ids},
            key=lambda wave_id: int(wave_id[1:]),
        )
        ordered_seed_ids = sorted(
            batch.included_seed_ids,
            key=lambda seed_id: (
                wave_index_by_id[seed_to_wave[seed_id]],
                seed_sort_key(seed_id, priorities),
            ),
        )
        entry_dependency_ids = parse_entry_dependencies(batch.entry_prerequisites)

        issue_templates: list[dict[str, object]] = []
        for seed_id in ordered_seed_ids:
            seed = seeds_by_id[seed_id]
            priority = priorities[seed_id]
            family_label = FAMILY_LABEL.get(seed.family, "unknown")
            worklane_label = WORKLANE_LABEL.get(seed.worklane, "unknown")
            owner_primary = DEFAULT_UNASSIGNED_OWNER
            owner_backup = DEFAULT_UNASSIGNED_OWNER
            if owner_assignments is not None:
                assignment = owner_assignments.get(seed.seed_id)
                if assignment is None:
                    raise ParseError(
                        f"owner map missing assignment for seed id: {seed.seed_id}"
                    )
                owner_primary = assignment.owner_primary
                owner_backup = assignment.owner_backup
            issue_templates.append(
                {
                    "seed_id": seed.seed_id,
                    "title": seed.proposed_issue_title,
                    "body_fields": {
                        "seed_id": seed.seed_id,
                        "family_tag": seed.family,
                        "worklane": seed.worklane,
                        "source_refs": [
                            "SRC-V013-12 tmp/reports/v013_future_work_seed_matrix.md"
                        ],
                        "depends_on": list(seed.depends_on),
                        "shard_class": seed.shard_class,
                        "objective": [split_seed_title_action(seed.proposed_issue_title)],
                        "artifact_targets": list(seed.artifact_targets),
                        "acceptance_gate_id": seed.acceptance_gate_id,
                        "validation_commands": [
                            "python scripts/spec_lint.py",
                            "python scripts/check_issue_checkbox_drift.py",
                        ],
                        "batch_id": batch.batch_id,
                        "owner_primary": owner_primary,
                        "owner_backup": owner_backup,
                    },
                    "labels": [
                        "phase:v013",
                        f"family:{family_label}",
                        f"lane:{worklane_label}",
                        f"seed:{seed.seed_id}",
                        f"shard:{seed.shard_class}",
                        f"priority:{priority.tier.lower()}",
                    ],
                    "priority": {
                        "priority_score": priority.priority_score,
                        "duv": priority.duv,
                        "dc": priority.dc,
                        "tier": priority.tier,
                    },
                    "wave_id": seed_to_wave[seed_id],
                }
            )

        min_wave_index = min(wave_index_by_id[wave_id] for wave_id in wave_coverage)
        max_wave_index = max(wave_index_by_id[wave_id] for wave_id in wave_coverage)
        batch_rows.append(
            {
                "batch_id": batch.batch_id,
                "class": batch.batch_class,
                "included_seed_ids": list(batch.included_seed_ids),
                "entry_prerequisites": batch.entry_prerequisites,
                "entry_dependency_ids": list(entry_dependency_ids),
                "exit_signal": batch.exit_signal,
                "wave_coverage": wave_coverage,
                "wave_span": {
                    "min_wave_id": f"W{min_wave_index}",
                    "max_wave_id": f"W{max_wave_index}",
                },
                "ordered_seed_ids": ordered_seed_ids,
                "issue_templates": issue_templates,
            }
        )

    batch_rows.sort(
        key=lambda row: (
            int(row["wave_span"]["min_wave_id"][1:]),
            CLASS_RANK.get(str(row["class"]), 99),
            str(row["batch_id"]),
        )
    )

    wave_to_batches: dict[str, list[str]] = {wave_id: [] for wave_id in wave_order}
    for batch in batch_rows:
        for wave_id in batch["wave_coverage"]:
            wave_to_batches[wave_id].append(str(batch["batch_id"]))

    payload = {
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
        "graph": {
            "seed_count": len(seed_rows),
            "edge_count": len(edge_rows),
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
                    "wave_id": seed_to_wave[row.seed_id],
                }
                for row in seed_rows
            ],
            "edges": [
                {
                    "edge_id": row.edge_id,
                    "predecessor": row.predecessor,
                    "successor": row.successor,
                    "type": row.edge_type,
                    "rationale": row.rationale,
                }
                for row in edge_rows
            ],
        },
        "wave_eligibility": wave_payload,
        "execution_order": execution_order,
        "batch_skeletons": {
            "batch_count": len(batch_rows),
            "batches": batch_rows,
            "wave_to_batches": [
                {
                    "wave_id": wave_id,
                    "batch_ids": wave_to_batches[wave_id],
                }
                for wave_id in wave_order
            ],
        },
    }

    return payload
