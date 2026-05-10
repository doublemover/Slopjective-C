from __future__ import annotations

import argparse
from pathlib import Path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="close_spt_issues_from_checkboxes.py",
        description=(
            "Detect checked checklist rows from the 510-task catalog and optionally "
            "close matching open SPT issues."
        ),
    )
    parser.add_argument(
        "--config",
        type=Path,
        default=None,
        help=(
            "Optional path to tooling config JSON. When omitted, built-in defaults are used; "
            "built-in defaults match pre-HB-06 behavior."
        ),
    )
    parser.add_argument(
        "--catalog",
        type=Path,
        default=None,
        help=(
            "Path to remaining_task_review_catalog.json. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--task-id-prefix",
        default=None,
        help=(
            "Only consider task IDs with this prefix. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--lane",
        choices=["A", "B", "C", "D"],
        default=None,
        help="Optional lane filter based on issue title tag '[Lane X]'.",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually close matched open issues. Default is dry-run.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional maximum number of issues to close/apply.",
    )
    parser.add_argument(
        "--commit-sha",
        default="",
        help="Optional commit SHA to include in closeout comment.",
    )
    parser.add_argument(
        "--plan-out",
        type=Path,
        default=None,
        help="Write immutable action-plan JSON and exit without closing issues.",
    )
    parser.add_argument(
        "--plan-in",
        type=Path,
        default=None,
        help="Read immutable action-plan JSON; required when --apply is set.",
    )
    parser.add_argument(
        "--fail-on-stale-source",
        action="store_true",
        help=(
            "Exit with code 2 if catalog source references are stale (missing file, line out of "
            "range, or source_line_hash mismatch)."
        ),
    )
    return parser


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    return build_parser().parse_args(argv)
