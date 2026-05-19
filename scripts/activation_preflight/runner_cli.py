"""Activation preflight runner command-line surface."""

from __future__ import annotations

import argparse
import textwrap
from pathlib import Path

from objc3c_tooling.paths import display_path
from scripts.activation_preflight.reports import (
    ACTIVATION_JSON_FILENAME,
    ACTIVATION_MD_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SNAPSHOT_CAPTURE_LOG_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
)
from scripts.activation_preflight.runner_paths import DEFAULT_CATALOG_JSON
from scripts.activation_preflight.runner_paths import DEFAULT_OPEN_BLOCKERS_ROOT
from scripts.activation_preflight.runner_paths import DEFAULT_OUTPUT_DIR
from scripts.activation_preflight.runner_paths import OPEN_BLOCKERS_REFRESH_RELATIVE_PATH
from scripts.activation_preflight.runner_state import parse_non_negative_int


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_activation_preflight.py",
        description=(
            "Run deterministic activation preflight orchestration "
            "(check_activation_triggers json+markdown + spec_lint) and persist evidence artifacts."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: gate closed and spec_lint succeeded.
              1: gate open (activation-open signal is preserved deterministically).
              2: orchestration error, malformed activation output, or spec_lint failure while gate is closed.

            Output artifact files (under --output-dir):
              - {ACTIVATION_JSON_FILENAME}
              - {ACTIVATION_MD_FILENAME}
              - {SPEC_LINT_LOG_FILENAME}
              - {SNAPSHOT_CAPTURE_LOG_FILENAME} (when --refresh-snapshots is used)
              - {OPEN_BLOCKERS_REFRESH_LOG_FILENAME} (when --refresh-open-blockers is used)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to offline issues snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to offline milestones snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--refresh-snapshots",
        action="store_true",
        help=(
            "Refresh issues/milestones snapshots before running activation checks by invoking "
            "scripts/capture_activation_snapshots.py."
        ),
    )
    parser.add_argument(
        "--snapshot-generated-at-utc",
        help=(
            "Optional generated_at_utc timestamp forwarded to capture_activation_snapshots "
            "when --refresh-snapshots is set."
        ),
    )
    parser.add_argument(
        "--issues-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for issues.",
    )
    parser.add_argument(
        "--milestones-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for milestones.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON passed to check_activation_triggers. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional open blockers JSON forwarded to check_activation_triggers "
            "for deterministic blocker-trigger gating. When --refresh-open-blockers "
            "is set and this path is omitted, the refreshed snapshot is written to "
            f"{OPEN_BLOCKERS_REFRESH_RELATIVE_PATH.as_posix()} under --output-dir."
        ),
    )
    parser.add_argument(
        "--refresh-open-blockers",
        action="store_true",
        help=(
            "Refresh open-blockers snapshot before activation checks by invoking "
            "scripts/extract_open_blockers.py with --format snapshot-json."
        ),
    )
    parser.add_argument(
        "--open-blockers-root",
        type=Path,
        default=DEFAULT_OPEN_BLOCKERS_ROOT,
        help=(
            "Root directory scanned for blocker rows when --refresh-open-blockers is set. "
            f"Default: {display_path(DEFAULT_OPEN_BLOCKERS_ROOT)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-generated-at-utc",
        help=(
            "generated_at_utc metadata forwarded to extract_open_blockers "
            "snapshot-json mode (required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--open-blockers-source",
        help=(
            "source metadata forwarded to extract_open_blockers snapshot-json mode "
            "(required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--actionable-status",
        action="append",
        dest="actionable_statuses",
        help=(
            "Repeatable actionable status forwarded to check_activation_triggers. "
            "Defaults are inherited when omitted."
        ),
    )
    t4_group = parser.add_mutually_exclusive_group()
    t4_group.add_argument(
        "--t4-governance-overlay-json",
        type=Path,
        help="Optional governance overlay JSON forwarded to check_activation_triggers.",
    )
    t4_group.add_argument(
        "--t4-new-scope-publish",
        action="store_true",
        help="Optional T4 override flag forwarded to check_activation_triggers.",
    )
    parser.add_argument(
        "--spec-glob",
        action="append",
        default=[],
        dest="spec_globs",
        help=(
            "Repeatable glob passed to spec_lint via --glob. "
            "If omitted, spec_lint defaults are used."
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


__all__ = ["build_parser"]
