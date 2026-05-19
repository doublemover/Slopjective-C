from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from execution_microtasks.catalog import load_issues
from execution_microtasks.catalog import validate_catalog_status_integrity
from execution_microtasks.dates import parse_generated_on
from execution_microtasks.dates import resolve_generated_on
from execution_microtasks.rendering import render_markdown

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CATALOG_JSON = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"


def parse_non_negative_int(raw: str) -> int:
    try:
        value = int(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer value: {raw!r}") from exc

    if value < 0:
        raise argparse.ArgumentTypeError("--closed-count must be >= 0")
    return value


def write_stdout(markdown: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(markdown.encode("utf-8"))
        return
    sys.stdout.write(markdown)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_execution_microtasks.py",
        description=(
            "Generate markdown for docs/reference/legacy_spec_anchor_index.md#execution-microtask-backlog from "
            "a GitHub issues JSON snapshot. When run without generation "
            "arguments, validates catalog execution_status integrity."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Expected JSON input comes from:\n"
            "  gh issue list --state open --json number,title,labels\n\n"
            "Example:\n"
            "  python scripts/generate_execution_microtasks.py \\\n"
            "    --issues-json tmp/open_issues.json \\\n"
            "    --closed-count 110 \\\n"
            "    --generated-on 2026-02-23 > docs/reference/legacy_spec_anchor_index.md#execution-microtask-backlog\n\n"
            "  SOURCE_DATE_EPOCH=1771804800 python scripts/generate_execution_microtasks.py \\\n"
            "    --issues-json tmp/open_issues.json \\\n"
            "    --closed-count 110 > docs/reference/legacy_spec_anchor_index.md#execution-microtask-backlog\n\n"
            "Status-integrity check mode:\n"
            "  python scripts/generate_execution_microtasks.py\n"
            "  python scripts/generate_execution_microtasks.py --allow-missing-status"
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        default=None,
        help=(
            "Path to a JSON array produced by "
            "`gh issue list --state open --json number,title,labels`."
        ),
    )
    parser.add_argument(
        "--closed-count",
        type=parse_non_negative_int,
        default=None,
        help="Count of closed issues at snapshot time.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=None,
        help=(
            "Path to remaining_task_review_catalog.json used for status-integrity "
            "checks. Defaults to tmp/reports/remaining_task_review_catalog.json "
            "in check mode."
        ),
    )
    parser.add_argument(
        "--allow-missing-status",
        action="store_true",
        help=(
            "Allow missing/blank execution_status values during catalog integrity "
            "checks."
        ),
    )
    parser.add_argument(
        "--generated-on",
        type=parse_generated_on,
        default=None,
        help=(
            "Date stamp embedded in output (YYYY-MM-DD). "
            "If omitted, SOURCE_DATE_EPOCH is used when set."
        ),
    )
    parser.add_argument(
        "--snapshot-date",
        type=parse_generated_on,
        default=None,
        help=argparse.SUPPRESS,
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    generation_mode = args.issues_json is not None or args.closed_count is not None
    catalog_path = args.catalog_json if args.catalog_json is not None else DEFAULT_CATALOG_JSON

    try:
        if not generation_mode or args.catalog_json is not None:
            validate_catalog_status_integrity(
                catalog_path,
                allow_missing_status=args.allow_missing_status,
            )

        if not generation_mode:
            return 0

        if args.issues_json is None or args.closed_count is None:
            parser.error(
                "must provide both --issues-json and --closed-count when generating microtasks"
            )

        issues = load_issues(args.issues_json)
        generated_on = resolve_generated_on(
            generated_on=args.generated_on,
            snapshot_date=args.snapshot_date,
        )
    except ValueError as exc:
        parser.error(str(exc))

    markdown = render_markdown(
        issues=issues,
        closed_count=args.closed_count,
        generated_on=generated_on,
    )
    write_stdout(markdown)
    return 0
