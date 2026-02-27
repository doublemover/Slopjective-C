#!/usr/bin/env python3
"""Generate deterministic v0.13 seed DAG and batch skeleton output."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MATRIX_PATH = ROOT / "spec/planning/v013_future_work_seed_matrix.md"
DEFAULT_OUTPUT_PATH = ROOT / "spec/planning/v013_seed_dependency_graph.json"

SEED_ID_RE = re.compile(r"^V013-[A-Z]+-[0-9]{2}$")
EDGE_ID_RE = re.compile(r"^EDGE-V013-[0-9]{3}$")
WAVE_ID_RE = re.compile(r"^W[0-9]+$")
DATE_RE = re.compile(r"([0-9]{4}-[0-9]{2}-[0-9]{2})")
TOKEN_RE = re.compile(r"(V013-[A-Z]+-[0-9]{2}|BATCH-V013-[SML]-[0-9]{2})")
OWNER_DATE_RE = re.compile(r"^[0-9]{4}-[0-9]{2}-[0-9]{2}$")
OWNER_TOKEN_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._:+/-]*$")

SEED_TABLE_HEADER = (
    "| Seed ID | Family | Worklane | Proposed GH issue title | Primary artifact targets | "
    "Depends on | Shard class | Acceptance gate ID |"
)
EDGE_TABLE_HEADER = "| Edge ID | Predecessor | Successor | Type | Rationale |"
WAVE_TABLE_HEADER = (
    "| Wave | Seeds eligible for execution (all hard predecessors satisfied) |"
)
BATCH_TABLE_HEADER = (
    "| Batch ID | Class | Included seed IDs | Entry prerequisites | Exit signal |"
)
PRIORITY_TABLE_HEADER = (
    "| Seed ID | CPI | DUV | RBV | ERC | ECP | DC | Priority score | Tier |"
)

REQUIRED_EDGE_IDS = [f"EDGE-V013-{index:03d}" for index in range(1, 22)]
REQUIRED_WAVE_IDS = [f"W{index}" for index in range(0, 8)]
OWNER_MAP_CONTRACT_ID = "V013-SEED-OWNER-REGISTRY-v1"
OWNER_MAP_SEED_ID = "V013-TOOL-03"
INVALID_OWNER_VALUES = {
    "na",
    "n/a",
    "none",
    "tbd",
    "todo",
    "unknown",
    "unassigned",
}

CLASS_RANK = {
    "small": 0,
    "medium": 1,
    "large": 2,
}

FAMILY_LABEL = {
    "FAM-SPEC": "spec",
    "FAM-TOOL": "tooling",
    "FAM-GOV": "governance",
    "FAM-REL": "release",
    "FAM-CONF": "conformance",
}

WORKLANE_LABEL = {
    "WL-SPEC": "spec",
    "WL-TOOL": "tooling",
    "WL-GOV": "governance",
    "WL-REL": "release",
    "WL-CONF": "conformance",
}


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


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_seed_batches.py",
        description=(
            "Generate deterministic seed DAG output and batch skeletons from "
            "spec/planning/v013_future_work_seed_matrix.md."
        ),
    )
    parser.add_argument(
        "--matrix",
        type=Path,
        default=DEFAULT_MATRIX_PATH,
        help="Path to v0.13 seed matrix markdown source.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT_PATH,
        help="Output JSON path for generated seed DAG and batch skeletons.",
    )
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="Print generated JSON to stdout after writing the output file.",
    )
    parser.add_argument(
        "--owner-map-json",
        type=Path,
        default=None,
        help=(
            "Optional path to deterministic seed owner registry JSON. "
            "When provided, every seed must have valid owner_primary and owner_backup values."
        ),
    )
    return parser


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def sanitize_cell(value: str) -> str:
    cleaned = value.strip()
    cleaned = cleaned.strip("`").strip()
    return cleaned


def split_markdown_row(line: str) -> list[str]:
    stripped = line.strip()
    if not stripped.startswith("|"):
        raise ParseError(f"expected markdown table row, got: {line!r}")
    cells = [cell.strip() for cell in stripped.strip("|").split("|")]
    return cells


def is_separator_row(cells: list[str]) -> bool:
    for cell in cells:
        marker = cell.replace("-", "").replace(":", "").strip()
        if marker:
            return False
    return True


def extract_table(lines: list[str], header: str, expected_columns: int) -> list[list[str]]:
    for index, line in enumerate(lines):
        if line.strip() != header:
            continue

        cursor = index + 1
        while cursor < len(lines) and not lines[cursor].strip():
            cursor += 1

        if cursor >= len(lines) or not lines[cursor].strip().startswith("|"):
            raise ParseError(f"missing separator row after table header: {header}")

        rows: list[list[str]] = []
        cursor += 1
        while cursor < len(lines):
            candidate = lines[cursor].strip()
            if not candidate.startswith("|"):
                break

            cells = split_markdown_row(lines[cursor])
            if len(cells) != expected_columns:
                raise ParseError(
                    f"table row has {len(cells)} columns; expected {expected_columns}: "
                    f"{lines[cursor]!r}"
                )
            if not is_separator_row(cells):
                rows.append([sanitize_cell(cell) for cell in cells])
            cursor += 1

        if not rows:
            raise ParseError(f"table has no data rows: {header}")
        return rows

    raise ParseError(f"table header not found: {header}")


def parse_id_list(cell: str) -> tuple[str, ...]:
    cleaned = sanitize_cell(cell)
    if not cleaned or cleaned == "none":
        return ()
    values: list[str] = []
    for part in cleaned.split(","):
        token = sanitize_cell(part)
        if token:
            values.append(token)
    return tuple(values)


def parse_artifact_targets(cell: str) -> tuple[str, ...]:
    targets = parse_id_list(cell)
    return tuple(target for target in targets if target)


def parse_snapshot_date(lines: list[str]) -> str:
    for line in lines[:20]:
        match = DATE_RE.search(line)
        if match:
            return match.group(1)
    raise ParseError("could not find snapshot date in matrix header")


def parse_seed_rows(lines: list[str]) -> list[SeedRow]:
    rows = extract_table(lines, SEED_TABLE_HEADER, expected_columns=8)
    parsed: list[SeedRow] = []
    seen_seed_ids: set[str] = set()
    for row in rows:
        seed_id = row[0]
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"invalid seed id in seed table: {seed_id}")
        if seed_id in seen_seed_ids:
            raise ParseError(f"duplicate seed id in seed table: {seed_id}")
        seen_seed_ids.add(seed_id)

        depends_on = parse_id_list(row[5])
        for dep in depends_on:
            if not SEED_ID_RE.match(dep):
                raise ParseError(f"invalid dependency seed id for {seed_id}: {dep}")

        parsed.append(
            SeedRow(
                seed_id=seed_id,
                family=row[1],
                worklane=row[2],
                proposed_issue_title=row[3],
                artifact_targets=parse_artifact_targets(row[4]),
                depends_on=depends_on,
                shard_class=row[6],
                acceptance_gate_id=row[7],
            )
        )
    return parsed


def parse_edge_rows(lines: list[str]) -> list[EdgeRow]:
    rows = extract_table(lines, EDGE_TABLE_HEADER, expected_columns=5)
    parsed: list[EdgeRow] = []
    seen_edge_ids: set[str] = set()
    for row in rows:
        edge_id = row[0]
        if not EDGE_ID_RE.match(edge_id):
            raise ParseError(f"invalid edge id in dependency table: {edge_id}")
        if edge_id in seen_edge_ids:
            raise ParseError(f"duplicate edge id in dependency table: {edge_id}")
        seen_edge_ids.add(edge_id)

        parsed.append(
            EdgeRow(
                edge_id=edge_id,
                predecessor=row[1],
                successor=row[2],
                edge_type=row[3],
                rationale=row[4],
            )
        )
    return parsed


def parse_wave_rows(lines: list[str]) -> dict[str, tuple[str, ...]]:
    rows = extract_table(lines, WAVE_TABLE_HEADER, expected_columns=2)
    waves: dict[str, tuple[str, ...]] = {}
    for row in rows:
        wave_id = row[0]
        if not WAVE_ID_RE.match(wave_id):
            raise ParseError(f"invalid wave id in wave table: {wave_id}")
        if wave_id in waves:
            raise ParseError(f"duplicate wave id in wave table: {wave_id}")
        waves[wave_id] = parse_id_list(row[1])
    return waves


def parse_batch_rows(lines: list[str]) -> list[BatchRow]:
    rows = extract_table(lines, BATCH_TABLE_HEADER, expected_columns=5)
    parsed: list[BatchRow] = []
    seen_batch_ids: set[str] = set()
    for row in rows:
        batch_id = row[0]
        if batch_id in seen_batch_ids:
            raise ParseError(f"duplicate batch id in batch table: {batch_id}")
        seen_batch_ids.add(batch_id)

        batch_class = row[1]
        if batch_class not in CLASS_RANK:
            raise ParseError(f"invalid batch class for {batch_id}: {batch_class}")

        included_seed_ids = parse_id_list(row[2])
        if not included_seed_ids:
            raise ParseError(f"batch {batch_id} must include at least one seed id")

        parsed.append(
            BatchRow(
                batch_id=batch_id,
                batch_class=batch_class,
                included_seed_ids=included_seed_ids,
                entry_prerequisites=row[3],
                exit_signal=row[4],
            )
        )
    return parsed


def parse_priority_rows(lines: list[str]) -> dict[str, PriorityRow]:
    rows = extract_table(lines, PRIORITY_TABLE_HEADER, expected_columns=9)
    parsed: dict[str, PriorityRow] = {}
    for row in rows:
        seed_id = row[0]
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"invalid seed id in priority table: {seed_id}")
        if seed_id in parsed:
            raise ParseError(f"duplicate seed id in priority table: {seed_id}")

        try:
            cpi = int(row[1])
            duv = int(row[2])
            rbv = int(row[3])
            erc = int(row[4])
            ecp = int(row[5])
            dc = int(row[6])
            priority_score = int(row[7])
        except ValueError as exc:
            raise ParseError(f"non-integer priority field for {seed_id}") from exc

        parsed[seed_id] = PriorityRow(
            seed_id=seed_id,
            cpi=cpi,
            duv=duv,
            rbv=rbv,
            erc=erc,
            ecp=ecp,
            dc=dc,
            priority_score=priority_score,
            tier=row[8],
        )
    return parsed


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


def parse_nonempty_string(value: Any, *, context: str) -> str:
    if not isinstance(value, str):
        raise ParseError(f"{context} must be a string")
    cleaned = value.strip()
    if not cleaned:
        raise ParseError(f"{context} must be a non-empty string")
    return cleaned


def parse_owner_value(value: Any, *, context: str) -> str:
    owner = parse_nonempty_string(value, context=context)
    lowered = owner.lower()
    if lowered in INVALID_OWNER_VALUES:
        raise ParseError(f"{context} must not be placeholder value {owner!r}")
    if not OWNER_TOKEN_RE.match(owner):
        raise ParseError(f"{context} has invalid owner token: {owner!r}")
    return owner


def load_owner_map(owner_map_path: Path) -> OwnerMapContract:
    try:
        payload_text = owner_map_path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise ParseError(f"owner map file not found: {owner_map_path}") from exc

    try:
        payload = json.loads(payload_text)
    except json.JSONDecodeError as exc:
        raise ParseError(f"owner map is not valid JSON: {owner_map_path}") from exc

    if not isinstance(payload, dict):
        raise ParseError("owner map root must be an object")

    contract_id = parse_nonempty_string(
        payload.get("contract_id"), context="owner map contract_id"
    )
    if contract_id != OWNER_MAP_CONTRACT_ID:
        raise ParseError(
            f"owner map contract_id must be {OWNER_MAP_CONTRACT_ID}; got {contract_id!r}"
        )

    snapshot_date = parse_nonempty_string(
        payload.get("snapshot_date"), context="owner map snapshot_date"
    )
    if not OWNER_DATE_RE.match(snapshot_date):
        raise ParseError(
            f"owner map snapshot_date must match YYYY-MM-DD; got {snapshot_date!r}"
        )

    source_matrix_path = parse_nonempty_string(
        payload.get("source_matrix_path"), context="owner map source_matrix_path"
    )
    seed_id = parse_nonempty_string(payload.get("seed_id"), context="owner map seed_id")
    if seed_id != OWNER_MAP_SEED_ID:
        raise ParseError(
            f"owner map seed_id must be {OWNER_MAP_SEED_ID}; got {seed_id!r}"
        )

    owner_registry_raw = payload.get("owner_registry")
    if not isinstance(owner_registry_raw, list) or not owner_registry_raw:
        raise ParseError("owner map owner_registry must be a non-empty array")

    owner_registry: dict[str, SeedOwnerAssignment] = {}
    seed_ids_in_order: list[str] = []
    for index, row in enumerate(owner_registry_raw):
        context = f"owner map owner_registry[{index}]"
        if not isinstance(row, dict):
            raise ParseError(f"{context} must be an object")

        seed_id = parse_nonempty_string(row.get("seed_id"), context=f"{context}.seed_id")
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"{context}.seed_id has invalid format: {seed_id!r}")
        if seed_id in owner_registry:
            raise ParseError(f"duplicate seed id in owner map: {seed_id}")

        owner_primary = parse_owner_value(
            row.get("owner_primary"), context=f"{context}.owner_primary"
        )
        owner_backup = parse_owner_value(
            row.get("owner_backup"), context=f"{context}.owner_backup"
        )

        seed_ids_in_order.append(seed_id)
        owner_registry[seed_id] = SeedOwnerAssignment(
            seed_id=seed_id,
            owner_primary=owner_primary,
            owner_backup=owner_backup,
        )

    if seed_ids_in_order != sorted(seed_ids_in_order):
        raise ParseError("owner map owner_registry must be sorted by seed_id ascending")

    return OwnerMapContract(
        contract_id=contract_id,
        seed_id=seed_id,
        snapshot_date=snapshot_date,
        source_matrix_path=source_matrix_path,
        owner_registry=owner_registry,
    )


def validate_owner_map_against_seeds(
    *,
    owner_map: OwnerMapContract,
    matrix_path: Path,
    matrix_snapshot_date: str,
    seed_ids: set[str],
) -> dict[str, SeedOwnerAssignment]:
    expected_source_matrix_path = display_path(matrix_path.resolve())
    if owner_map.source_matrix_path != expected_source_matrix_path:
        raise ParseError(
            "owner map source_matrix_path mismatch; "
            f"expected {expected_source_matrix_path!r}, "
            f"got {owner_map.source_matrix_path!r}"
        )

    if owner_map.snapshot_date != matrix_snapshot_date:
        raise ParseError(
            "owner map snapshot_date mismatch; "
            f"expected {matrix_snapshot_date!r}, "
            f"got {owner_map.snapshot_date!r}"
        )

    owner_seed_ids = set(owner_map.owner_registry.keys())
    missing_seed_ids = sorted(seed_ids - owner_seed_ids)
    if missing_seed_ids:
        raise ParseError(
            "owner map missing seed id(s): " + ", ".join(missing_seed_ids)
        )

    unknown_seed_ids = sorted(owner_seed_ids - seed_ids)
    if unknown_seed_ids:
        raise ParseError(
            "owner map contains unknown seed id(s): " + ", ".join(unknown_seed_ids)
        )

    return owner_map.owner_registry


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
            owner_primary = "TBD"
            owner_backup = "TBD"
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
                            "SRC-V013-12 spec/planning/v013_future_work_seed_matrix.md"
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


def generate(
    matrix_path: Path,
    output_path: Path,
    print_stdout: bool,
    owner_map_path: Path | None = None,
) -> int:
    try:
        source_text = matrix_path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise ParseError(f"matrix file not found: {matrix_path}") from exc

    lines = source_text.splitlines()
    snapshot_date = parse_snapshot_date(lines)
    seeds = parse_seed_rows(lines)
    edges = parse_edge_rows(lines)
    waves = parse_wave_rows(lines)
    batches = parse_batch_rows(lines)
    priorities = parse_priority_rows(lines)
    owner_assignments: dict[str, SeedOwnerAssignment] | None = None
    if owner_map_path is not None:
        owner_map = load_owner_map(owner_map_path)
        owner_assignments = validate_owner_map_against_seeds(
            owner_map=owner_map,
            matrix_path=matrix_path,
            matrix_snapshot_date=snapshot_date,
            seed_ids={seed.seed_id for seed in seeds},
        )

    payload = build_payload(
        matrix_path=matrix_path.resolve(),
        snapshot_date=snapshot_date,
        seeds=seeds,
        edges=edges,
        waves=waves,
        batches=batches,
        priorities=priorities,
        owner_assignments=owner_assignments,
    )

    rendered = json.dumps(payload, indent=2) + "\n"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(rendered, encoding="utf-8")

    rel_matrix = display_path(matrix_path)
    rel_output = display_path(output_path)
    print(
        "seed-dag: OK "
        f"(matrix={rel_matrix}, output={rel_output}, "
        f"seeds={payload['graph']['seed_count']}, edges={payload['graph']['edge_count']}, "
        f"waves={len(payload['wave_eligibility'])}, batches={payload['batch_skeletons']['batch_count']})"
    )

    if print_stdout:
        sys.stdout.write(rendered)

    return 0


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return generate(
            matrix_path=args.matrix,
            output_path=args.output,
            print_stdout=args.stdout,
            owner_map_path=args.owner_map_json,
        )
    except ParseError as exc:
        print(f"seed-dag: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
