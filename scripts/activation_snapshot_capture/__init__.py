"""Activation snapshot capture support package."""

from __future__ import annotations

from activation_snapshot_capture.artifact_loading import (
    normalize_issue_item,
    normalize_milestone_item,
    parse_labels,
    parse_milestone_ref,
    parse_optional_non_negative_int,
    parse_optional_str,
    parse_required_number,
)
from activation_snapshot_capture.cli import build_parser, capture_snapshots, main
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
    ROOT,
    SCRIPT_NAME,
    SCRIPTS_DIR,
)
from activation_snapshot_capture.models import (
    GhClientFactory,
    GhClientLike,
    SnapshotCaptureResult,
    SnapshotError,
    SnapshotOutputPaths,
)
from activation_snapshot_capture.paths import resolve_snapshot_outputs
from activation_snapshot_capture.snapshot_shaping import (
    build_snapshot,
    parse_generated_at_utc,
    resolve_generated_at_utc,
    sort_items_by_number,
    source_date_epoch_to_generated_at_utc,
)

__all__ = [
    "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
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
    "main",
    "normalize_issue_item",
    "normalize_milestone_item",
    "parse_generated_at_utc",
    "parse_labels",
    "parse_milestone_ref",
    "parse_optional_non_negative_int",
    "parse_optional_str",
    "parse_required_number",
    "resolve_generated_at_utc",
    "resolve_snapshot_outputs",
    "sort_items_by_number",
    "source_date_epoch_to_generated_at_utc",
]
