"""Command-line interface for open-issue extraction."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence, TextIO

from .config import DEFAULT_SPEC_DIR, config_from_args
from .models import ParseIssue
from .orchestration import collect_open_issues
from .rendering import render_report


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Extract '## ... Open issues' sections from spec/PART_*.md."
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Exit with status 1 if malformed open-issues sections are detected.",
    )
    parser.add_argument(
        "--spec-dir",
        type=Path,
        default=DEFAULT_SPEC_DIR,
        help=f"Directory containing PART_*.md files (default: {DEFAULT_SPEC_DIR}).",
    )
    return parser


def write_parse_issues(parse_issues: Sequence[ParseIssue], stream: TextIO) -> None:
    for issue in parse_issues:
        print(f"{issue.file}:{issue.line}: {issue.message}", file=stream)


def main(argv: Sequence[str] | None = None) -> int:
    config = config_from_args(build_parser().parse_args(argv))
    result = collect_open_issues(config.spec_dir)

    sys.stdout.write(render_report(result.records, config.output_format))
    write_parse_issues(result.parse_issues, sys.stderr)

    if config.strict and result.parse_issues:
        return 1
    return 0


__all__ = (
    "build_parser",
    "main",
    "write_parse_issues",
)
