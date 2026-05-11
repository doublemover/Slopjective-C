"""Markdown source parsing for seed matrix inputs."""

from __future__ import annotations

from ..models import BatchRow, EdgeRow, ParseError, PriorityRow, SeedRow
from .constants import (
    BATCH_TABLE_COLUMNS,
    CLASS_RANK,
    DATE_RE,
    EDGE_ID_RE,
    EDGE_TABLE_COLUMNS,
    PRIORITY_TABLE_COLUMNS,
    SEED_ID_RE,
    SEED_TABLE_COLUMNS,
    WAVE_ID_RE,
    WAVE_TABLE_COLUMNS,
)


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


def extract_table(lines: list[str], header_columns: tuple[str, ...]) -> list[list[str]]:
    expected_columns = len(header_columns)
    header_label = "| " + " | ".join(header_columns) + " |"
    for index, line in enumerate(lines):
        candidate = line.strip()
        if not candidate.startswith("|"):
            continue
        cells = split_markdown_row(candidate)
        if len(cells) != expected_columns:
            continue
        normalized_cells = tuple(sanitize_cell(cell) for cell in cells)
        if normalized_cells != header_columns:
            continue

        cursor = index + 1
        while cursor < len(lines) and not lines[cursor].strip():
            cursor += 1

        if cursor >= len(lines) or not lines[cursor].strip().startswith("|"):
            raise ParseError(f"missing separator row after table header: {header_label}")

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
            raise ParseError(f"table has no data rows: {header_label}")
        return rows

    raise ParseError(f"table header not found: {header_label}")


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
    rows = extract_table(lines, SEED_TABLE_COLUMNS)
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
    rows = extract_table(lines, EDGE_TABLE_COLUMNS)
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
    rows = extract_table(lines, WAVE_TABLE_COLUMNS)
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
    rows = extract_table(lines, BATCH_TABLE_COLUMNS)
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
    rows = extract_table(lines, PRIORITY_TABLE_COLUMNS)
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
