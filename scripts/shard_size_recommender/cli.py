from __future__ import annotations

import argparse
import json
import sys

from .errors import RecommendError
from .models import RecommenderInputs
from .payloads import build_payload
from .policy import recommend
from .rendering import render_text


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="recommend_shard_size.py",
        description=(
            "Evaluate deterministic shard-size guidance calibrated from v0.12 "
            "Wave 10 and Wave 16 execution history."
        ),
    )
    parser.add_argument(
        "--id-count",
        type=int,
        required=True,
        help="Planned checklist ID count for the shard candidate (must be >= 1).",
    )
    parser.add_argument(
        "--primary-files",
        type=int,
        required=True,
        help="Primary artifact file count expected to be edited (must be >= 1).",
    )
    parser.add_argument(
        "--hard-dependency-count",
        type=int,
        required=True,
        help="Hard predecessor dependency count (must be >= 0).",
    )
    parser.add_argument(
        "--lane-footprint",
        type=int,
        required=True,
        help="Distinct worklane count touched by the shard candidate (must be >= 1).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "text"),
        default="json",
        help="Render output as machine-readable JSON or compact text summary.",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    inputs = RecommenderInputs(
        id_count=args.id_count,
        primary_files=args.primary_files,
        hard_dependency_count=args.hard_dependency_count,
        lane_footprint=args.lane_footprint,
    )

    try:
        recommendation = recommend(inputs)
        payload = build_payload(inputs=inputs, recommendation=recommendation)
    except RecommendError as exc:
        print(f"recommend-shard-size: {exc}", file=sys.stderr)
        return 2

    if args.format == "text":
        sys.stdout.write(render_text(payload))
    else:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    return 0
