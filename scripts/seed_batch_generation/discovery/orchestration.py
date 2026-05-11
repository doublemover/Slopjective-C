"""Public orchestration for seed matrix discovery."""

from __future__ import annotations

from ..models import SeedMatrix
from .source_parsing import (
    parse_batch_rows,
    parse_edge_rows,
    parse_priority_rows,
    parse_seed_rows,
    parse_snapshot_date,
    parse_wave_rows,
)


def parse_seed_matrix(source_text: str) -> SeedMatrix:
    lines = source_text.splitlines()
    return SeedMatrix(
        snapshot_date=parse_snapshot_date(lines),
        seeds=parse_seed_rows(lines),
        edges=parse_edge_rows(lines),
        waves=parse_wave_rows(lines),
        batches=parse_batch_rows(lines),
        priorities=parse_priority_rows(lines),
    )
