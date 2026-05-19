"""Argument parser for remaining-task backlog extraction."""

from __future__ import annotations

import argparse
from pathlib import Path

from objc3c_tooling.paths import display_path
from remaining_task_extraction.cli_paths import DEFAULT_INPUT
from remaining_task_extraction.model import DEFAULT_MAX_OVERLAP_CONFLICTS

__all__ = [
    "build_parser",
]


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="extract_remaining_tasks.py",
        description=(
            "Read remaining_task_review_catalog.json and emit deterministic backlog views."
        ),
    )
    parser.add_argument(
        "--input",
        type=Path,
        default=DEFAULT_INPUT,
        help=(
            "Path to remaining_task_review_catalog.json "
            f"(default: {display_path(DEFAULT_INPUT)})."
        ),
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    parser.add_argument(
        "--status",
        action="append",
        dest="status_filters",
        help=(
            "Repeatable execution status filter. "
            "Defaults to: open, open-blocked, blocked."
        ),
    )
    parser.add_argument(
        "--lane",
        action="append",
        dest="lane_filters",
        help="Repeatable lane filter (for example: A, B, C, D).",
    )
    parser.add_argument(
        "--group-by",
        choices=("lane", "status", "path"),
        default="lane",
        help="Group tasks by lane (default), status, or path.",
    )
    parser.add_argument(
        "--allow-missing-status",
        action="store_true",
        help=(
            "Allow missing/blank execution_status values and normalize them to "
            "'missing'."
        ),
    )
    parser.add_argument(
        "--max-dispatch-intake-status",
        choices=("pass", "drift", "fail"),
        help=(
            "Enforcement guard: exit non-zero when computed dispatch-intake status "
            "is more severe than this value."
        ),
    )
    parser.add_argument(
        "--max-overlap-conflicts",
        type=int,
        default=DEFAULT_MAX_OVERLAP_CONFLICTS,
        help=(
            "Classify overlap conflict severity using this threshold: "
            "0 conflicts=pass, 1..N conflicts=drift, >N conflicts=fail."
        ),
    )
    return parser
