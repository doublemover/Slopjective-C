"""Bootstrap readiness command-line interface."""

from __future__ import annotations

import argparse
import textwrap
from pathlib import Path

from objc3c_tooling.paths import display_path

from .constants import (
    DEFAULT_CATALOG_JSON,
    DEFAULT_OPEN_BLOCKERS_ROOT,
    DEFAULT_OUTPUT_DIR,
)
from .reports import (
    BOOTSTRAP_JSON_FILENAME,
    BOOTSTRAP_JSON_LOG_FILENAME,
    BOOTSTRAP_MD_FILENAME,
    BOOTSTRAP_MD_LOG_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_bootstrap_readiness.py",
        description=(
            "Run deterministic bootstrap readiness orchestration "
            "(check_bootstrap_readiness json+markdown with optional refresh/spec_lint) "
            "and persist evidence artifacts."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: bootstrappable readiness and no runner/contract errors.
              1: blocked readiness and no runner/contract errors.
              2: runner error, command contract mismatch, or optional command failure.

            Output artifact files (under --output-dir):
              - {BOOTSTRAP_JSON_FILENAME}
              - {BOOTSTRAP_MD_FILENAME}
              - {BOOTSTRAP_JSON_LOG_FILENAME}
              - {BOOTSTRAP_MD_LOG_FILENAME}
              - {OPEN_BLOCKERS_REFRESH_LOG_FILENAME} (when --refresh-open-blockers is used)
              - {SPEC_LINT_LOG_FILENAME} (when --run-spec-lint is used)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to open-issues snapshot JSON passed to check_bootstrap_readiness.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to open-milestones snapshot JSON passed to check_bootstrap_readiness.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON passed to check_bootstrap_readiness. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional open blockers JSON passed to check_bootstrap_readiness. When "
            "--refresh-open-blockers is set and this path is omitted, a refreshed snapshot "
            "is written under --output-dir/inputs/open_blockers.snapshot.json."
        ),
    )
    parser.add_argument(
        "--refresh-open-blockers",
        action="store_true",
        help=(
            "Refresh open blockers before readiness checks by invoking "
            "scripts/extract_open_blockers.py with --format snapshot-json."
        ),
    )
    parser.add_argument(
        "--open-blockers-root",
        type=Path,
        default=DEFAULT_OPEN_BLOCKERS_ROOT,
        help=(
            "Root directory scanned when --refresh-open-blockers is used. "
            f"Default: {display_path(DEFAULT_OPEN_BLOCKERS_ROOT)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-generated-at-utc",
        help=(
            "generated_at_utc metadata forwarded to extract_open_blockers snapshot-json "
            "(required when --refresh-open-blockers is used)."
        ),
    )
    parser.add_argument(
        "--open-blockers-source",
        help=(
            "source metadata forwarded to extract_open_blockers snapshot-json "
            "(required when --refresh-open-blockers is used)."
        ),
    )
    parser.add_argument(
        "--run-spec-lint",
        action="store_true",
        help="Run scripts/spec_lint.py after readiness checks.",
    )
    parser.add_argument(
        "--spec-glob",
        action="append",
        default=[],
        dest="spec_globs",
        help=(
            "Repeatable glob passed to spec_lint via --glob when --run-spec-lint is set. "
            "When omitted, spec_lint defaults are used."
        ),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=(
            "Directory where deterministic artifacts are written. "
            f"Default: {display_path(DEFAULT_OUTPUT_DIR)}."
        ),
    )
    return parser
