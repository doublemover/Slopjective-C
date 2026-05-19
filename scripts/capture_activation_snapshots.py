#!/usr/bin/env python3
"""Stable entrypoint for activation snapshot capture."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from activation_snapshot_capture.artifact_loading import (
    normalize_issue_item,
    normalize_milestone_item,
    parse_labels,
    parse_milestone_ref,
    parse_optional_non_negative_int,
    parse_optional_str,
    parse_required_number,
)
from activation_snapshot_capture.cli import build_parser, capture_snapshots
from activation_snapshot_capture.cli import main as _package_main
from activation_snapshot_capture.command_capture import (
    collect_open_issues,
    collect_open_milestones,
)
from activation_snapshot_capture.constants import (
    CAPTURE_SNAPSHOTS_SCRIPT_PATH,
    ISSUES_ENDPOINT,
    ISSUES_SOURCE,
    MILESTONES_ENDPOINT,
    MILESTONES_SOURCE,
    PROGRAM_NAME,
    SCRIPT_NAME,
)
from activation_snapshot_capture.models import (
    GhClientFactory,
    GhClientLike,
    SnapshotCaptureResult,
    SnapshotError,
    SnapshotOutputPaths,
)
from activation_snapshot_capture.paths import ROOT, SCRIPTS_DIR, resolve_snapshot_outputs
from activation_snapshot_capture.snapshot_shaping import (
    build_snapshot,
    parse_generated_at_utc,
    resolve_generated_at_utc,
    sort_items_by_number,
    source_date_epoch_to_generated_at_utc,
)
from lib.gh_client import GhClient, GhClientError
from objc3c_tooling.json_io import render_json
from objc3c_tooling.json_io import write_text_file as write_text
from objc3c_tooling.paths import display_path, resolve_repo_path


def main(argv: list[str] | None = None) -> int:
    return _package_main(argv, client_cls=GhClient)


__all__ = [
    "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
    "GhClient",
    "GhClientError",
    "GhClientFactory",
    "GhClientLike",
    "ISSUES_ENDPOINT",
    "ISSUES_SOURCE",
    "MILESTONES_ENDPOINT",
    "MILESTONES_SOURCE",
    "PROGRAM_NAME",
    "ROOT",
    "SCRIPT_NAME",
    "SCRIPTS_DIR",
    "SnapshotCaptureResult",
    "SnapshotError",
    "SnapshotOutputPaths",
    "build_parser",
    "build_snapshot",
    "capture_snapshots",
    "collect_open_issues",
    "collect_open_milestones",
    "display_path",
    "main",
    "normalize_issue_item",
    "normalize_milestone_item",
    "parse_generated_at_utc",
    "parse_labels",
    "parse_milestone_ref",
    "parse_optional_non_negative_int",
    "parse_optional_str",
    "parse_required_number",
    "render_json",
    "resolve_generated_at_utc",
    "resolve_repo_path",
    "resolve_snapshot_outputs",
    "sort_items_by_number",
    "source_date_epoch_to_generated_at_utc",
    "write_text",
]


if __name__ == "__main__":
    raise SystemExit(main())
