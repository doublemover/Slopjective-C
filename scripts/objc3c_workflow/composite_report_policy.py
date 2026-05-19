"""Owned policy constants and formatting for composite workflow reports."""

from __future__ import annotations

from pathlib import Path

COMPOSITE_REPORT_POLICY_OWNER = "objc3c-workflow-composite-report-policy"
COMPOSITE_PROGRESS_POLICY_OWNER = "objc3c-workflow-composite-progress-policy"
COMPOSITE_STATUS_POLICY_OWNER = "objc3c-workflow-composite-status-policy"
COMPOSITE_STATUS_PASS = "PASS"
COMPOSITE_STATUS_FAIL = "FAIL"
COMPOSITE_PROGRESS_PREFIX = "public-workflow-progress"
COMPOSITE_REPORT_PREFIX = "public-workflow-report"


def composite_progress_start_line(
    *,
    index: int,
    total: int,
    action: str,
    previous_action: str,
    elapsed_seconds: float,
) -> str:
    return (
        f"{COMPOSITE_PROGRESS_PREFIX}: [{index}/{total}] START action={action} "
        f"elapsed={elapsed_seconds:.3f}s last={previous_action}"
    )


def composite_progress_done_line(
    *,
    index: int,
    total: int,
    action: str,
    duration_seconds: float,
    elapsed_seconds: float,
    exit_code: object,
) -> str:
    return (
        f"{COMPOSITE_PROGRESS_PREFIX}: [{index}/{total}] DONE action={action} "
        f"duration={duration_seconds:.3f}s elapsed={elapsed_seconds:.3f}s exit={exit_code}"
    )


def composite_report_announcement(root: Path, report_path: Path) -> str:
    return f"{COMPOSITE_REPORT_PREFIX}: {report_path.relative_to(root).as_posix()}"


__all__ = [
    "COMPOSITE_PROGRESS_POLICY_OWNER",
    "COMPOSITE_PROGRESS_PREFIX",
    "COMPOSITE_REPORT_POLICY_OWNER",
    "COMPOSITE_REPORT_PREFIX",
    "COMPOSITE_STATUS_FAIL",
    "COMPOSITE_STATUS_PASS",
    "COMPOSITE_STATUS_POLICY_OWNER",
    "composite_progress_done_line",
    "composite_progress_start_line",
    "composite_report_announcement",
]
