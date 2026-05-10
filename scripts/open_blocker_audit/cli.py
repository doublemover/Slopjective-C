"""Open-blocker audit command-line interface."""

from __future__ import annotations

import argparse
import textwrap
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from .commands import run_command
from .constants import DEFAULT_AUDIT_ROOT, DEFAULT_OUTPUT_DIR
from .rendering import (
    CONTRACT_CHECK_STDERR_FILENAME,
    CONTRACT_CHECK_TRANSCRIPT_FILENAME,
    DEFAULT_SNAPSHOT_RELATIVE_PATH,
    EXTRACT_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SUMMARY_JSON_FILENAME,
)
from .runner import CommandRunner, run_audit


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_open_blocker_audit.py",
        description=(
            "Run deterministic repo-root open blocker audit orchestration by invoking "
            "extract_open_blockers snapshot-json mode with explicit metadata, exclusions, "
            "schema checks, and fail-closed artifact persistence."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: no open blockers and no runner/contract errors.
              1: one or more open blockers discovered.
              2: runner error, extractor contract mismatch, or schema/provenance drift.

            Output artifact files (under --output-dir):
              - {DEFAULT_SNAPSHOT_RELATIVE_PATH.as_posix()}
              - {EXTRACT_LOG_FILENAME} (when extract_open_blockers is attempted)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
              - {CONTRACT_CHECK_TRANSCRIPT_FILENAME}
              - {CONTRACT_CHECK_STDERR_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--audit-root",
        type=Path,
        default=DEFAULT_AUDIT_ROOT,
        help=(
            "Root directory scanned for markdown blockers. "
            f"Default: {display_path(DEFAULT_AUDIT_ROOT)}."
        ),
    )
    parser.add_argument(
        "--generated-at-utc",
        help=(
            "Required strict UTC timestamp metadata for snapshot-json mode "
            "(YYYY-MM-DDTHH:MM:SSZ)."
        ),
    )
    parser.add_argument(
        "--source",
        help=(
            "Required canonical non-empty source metadata for snapshot-json mode "
            "(no leading/trailing or repeated internal whitespace)."
        ),
    )
    parser.add_argument(
        "--include-glob",
        action="append",
        default=[],
        dest="include_globs",
        help=(
            "Optional repository-relative markdown include glob. Repeatable. "
            "Patterns must share one static directory prefix "
            "(for example: docs/reference/**/*.md)."
        ),
    )
    parser.add_argument(
        "--exclude-path",
        action="append",
        default=[],
        dest="exclude_paths",
        help=(
            "Repeatable exclusion glob forwarded to extract_open_blockers --exclude-path. "
            "Defaults are applied unless --no-default-exclude is set."
        ),
    )
    parser.add_argument(
        "--no-default-exclude",
        action="store_true",
        help="Disable default exclusion globs and use only --exclude-path entries.",
    )
    parser.add_argument(
        "--snapshot-json",
        type=Path,
        help=(
            "Optional output path for normalized blocker snapshot JSON. "
            "Defaults to --output-dir/inputs/open_blockers.snapshot.json."
        ),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=(
            "Directory where deterministic audit artifacts are written. "
            f"Default: {display_path(DEFAULT_OUTPUT_DIR)}."
        ),
    )
    return parser


def run_cli(
    argv: Sequence[str] | None = None,
    *,
    command_runner: CommandRunner = run_command,
) -> int:
    return run_audit(build_parser().parse_args(argv), command_runner=command_runner)


__all__ = ["build_parser", "run_cli"]
