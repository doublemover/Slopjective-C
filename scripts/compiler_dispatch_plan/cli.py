from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.json_io import render_json
from objc3c_tooling.paths import display_path

from compiler_dispatch_plan.constants import DEFAULT_ISSUES_JSON
from compiler_dispatch_plan.loading import parse_issue_rows
from compiler_dispatch_plan.normalization import normalize_path
from compiler_dispatch_plan.planning import build_payload
from compiler_dispatch_plan.planning import pick_target_milestone
from compiler_dispatch_plan.rendering import render_markdown


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Generate deterministic lane-parallel compiler dispatch summaries."
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        default=DEFAULT_ISSUES_JSON,
        help=f"Path to gh open-issues snapshot (default: {display_path(DEFAULT_ISSUES_JSON)}).",
    )
    parser.add_argument(
        "--milestone-number",
        type=int,
        help="Optional explicit milestone number; defaults to the lowest open milestone in snapshot.",
    )
    parser.add_argument(
        "--top-n",
        type=int,
        default=3,
        help="Number of next tasks to emit per lane (default: 3).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format (default: json).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.top_n <= 0:
        print("error: --top-n must be > 0", file=sys.stderr)
        return 2

    issues_json_path = normalize_path(args.issues_json)
    try:
        rows = parse_issue_rows(issues_json_path)
        milestone_number = pick_target_milestone(rows, args.milestone_number)
        payload = build_payload(rows, milestone_number=milestone_number, top_n=args.top_n)
        payload["source"]["issues_json"] = display_path(issues_json_path)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.format == "markdown":
        sys.stdout.write(render_markdown(payload))
    else:
        sys.stdout.write(render_json(payload))
    return 0
