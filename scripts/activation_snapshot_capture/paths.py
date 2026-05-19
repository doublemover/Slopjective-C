"""Path discovery and output path normalization for snapshot capture."""

from __future__ import annotations

from pathlib import Path

from activation_snapshot_capture.constants import ROOT, SCRIPTS_DIR
from activation_snapshot_capture.models import SnapshotOutputPaths
from objc3c_tooling.paths import resolve_repo_path


def resolve_snapshot_outputs(
    *,
    issues_output: Path,
    milestones_output: Path,
) -> SnapshotOutputPaths:
    return SnapshotOutputPaths(
        issues_output=resolve_repo_path(issues_output),
        milestones_output=resolve_repo_path(milestones_output),
    )


__all__ = ["ROOT", "SCRIPTS_DIR", "resolve_snapshot_outputs"]
