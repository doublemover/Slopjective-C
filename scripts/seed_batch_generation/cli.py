"""CLI and output writing for seed batch generation."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from objc3c_tooling.paths import display_path

from .config import DEFAULT_MATRIX_PATH, DEFAULT_OUTPUT_PATH
from .discovery import parse_seed_matrix
from .models import ParseError, SeedOwnerAssignment
from .owners import load_owner_map, validate_owner_map_against_seeds
from .planning import build_payload


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_seed_batches.py",
        description=(
            "Generate deterministic seed DAG output and batch skeletons from "
            "tmp/reports/v013_future_work_seed_matrix.md."
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

    matrix = parse_seed_matrix(source_text)
    owner_assignments: dict[str, SeedOwnerAssignment] | None = None
    if owner_map_path is not None:
        owner_map = load_owner_map(owner_map_path)
        owner_assignments = validate_owner_map_against_seeds(
            owner_map=owner_map,
            matrix_path=matrix_path,
            matrix_snapshot_date=matrix.snapshot_date,
            seed_ids={seed.seed_id for seed in matrix.seeds},
        )

    payload = build_payload(
        matrix_path=matrix_path.resolve(),
        snapshot_date=matrix.snapshot_date,
        seeds=matrix.seeds,
        edges=matrix.edges,
        waves=matrix.waves,
        batches=matrix.batches,
        priorities=matrix.priorities,
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
