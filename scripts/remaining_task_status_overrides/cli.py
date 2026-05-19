from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from .applying import apply_overrides
from .catalog import load_catalog
from .json_files import write_catalog
from .override_loading import load_overrides
from .summary import render_summary


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="apply_remaining_task_status_overrides.py",
        description=(
            "Apply per-lane status override JSON files to "
            "tmp/reports/remaining_task_review_catalog.json."
        ),
    )
    parser.add_argument(
        "--catalog",
        type=Path,
        default=Path("tmp/reports/remaining_task_review_catalog.json"),
        help="Path to remaining_task_review_catalog.json.",
    )
    parser.add_argument(
        "--overrides",
        type=Path,
        nargs="+",
        required=True,
        help="One or more override JSON files.",
    )
    parser.add_argument(
        "--write",
        action="store_true",
        help="Write changes back to --catalog. Without this flag, dry-run only.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        overrides = load_overrides(args.overrides)
        catalog = load_catalog(args.catalog)
        changes, missing = apply_overrides(catalog, overrides)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(render_summary(changes, missing))

    if missing:
        return 2

    if args.write:
        write_catalog(args.catalog, catalog)
        print(f"- catalog_updated={args.catalog.as_posix()}")
    else:
        print("- dry_run=true")

    return 0
