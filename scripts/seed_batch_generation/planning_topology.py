"""Seed batch graph topology and ordering helpers."""

from __future__ import annotations

from .config import TOKEN_RE
from .models import EdgeRow, ParseError, PriorityRow, SeedRow


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


__all__ = [
    "compute_topological_waves",
    "normalize_wave_membership",
    "parse_entry_dependencies",
    "seed_sort_key",
    "split_seed_title_action",
    "validate_waves_against_topology",
]
