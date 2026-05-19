from __future__ import annotations

import argparse
import sys
from pathlib import Path

from seed_issue_payloads.model import DEFAULT_GRAPH_PATH
from seed_issue_payloads.model import DEFAULT_OUTPUT_PATH
from seed_issue_payloads.model import ParseError
from seed_issue_payloads.payload import generate


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_seed_issue_payloads.py",
        description=(
            "Generate deterministic issue payload records from "
            "tmp/reports/v013_seed_dependency_graph.json."
        ),
    )
    parser.add_argument(
        "--graph",
        type=Path,
        default=DEFAULT_GRAPH_PATH,
        help="Input seed dependency graph JSON path.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT_PATH,
        help="Output JSON path for generated issue payload records.",
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        help=(
            "Optional GitHub issue snapshot JSON array. "
            "When provided, records are overlaid with completion metadata "
            "for unambiguous seed/issue matches."
        ),
    )
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="Print generated JSON to stdout after writing output.",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return generate(
            graph_path=args.graph,
            output_path=args.output,
            print_stdout=args.stdout,
            issues_json_path=args.issues_json,
        )
    except ParseError as exc:
        print(f"seed-issue-payloads: {exc}", file=sys.stderr)
        return 1
