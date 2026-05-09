from __future__ import annotations

from pathlib import Path

from scripts.objc3c_workflow.composite_report_policy import (
    COMPOSITE_PROGRESS_POLICY_OWNER,
    COMPOSITE_REPORT_POLICY_OWNER,
    COMPOSITE_STATUS_FAIL,
    COMPOSITE_STATUS_PASS,
    COMPOSITE_STATUS_POLICY_OWNER,
    composite_progress_done_line,
    composite_progress_start_line,
    composite_report_announcement,
)
from scripts.objc3c_workflow.composite_report_status import effective_composite_status


def test_composite_report_policy_owns_status_and_progress_text() -> None:
    assert COMPOSITE_REPORT_POLICY_OWNER == "objc3c-workflow-composite-report-policy"
    assert COMPOSITE_PROGRESS_POLICY_OWNER == "objc3c-workflow-composite-progress-policy"
    assert COMPOSITE_STATUS_POLICY_OWNER == "objc3c-workflow-composite-status-policy"
    assert COMPOSITE_STATUS_PASS == "PASS"
    assert COMPOSITE_STATUS_FAIL == "FAIL"
    assert composite_progress_start_line(
        index=1,
        total=2,
        action="lint",
        previous_action="none",
        elapsed_seconds=1.25,
    ) == "public-workflow-progress: [1/2] START action=lint elapsed=1.250s last=none"
    assert composite_progress_done_line(
        index=1,
        total=2,
        action="lint",
        duration_seconds=0.5,
        elapsed_seconds=1.5,
        exit_code=0,
    ) == "public-workflow-progress: [1/2] DONE action=lint duration=0.500s elapsed=1.500s exit=0"


def test_composite_report_policy_owns_announcement_and_fail_closed_status() -> None:
    assert composite_report_announcement(Path("repo"), Path("repo/tmp/report.json")) == (
        "public-workflow-report: tmp/report.json"
    )
    assert effective_composite_status(COMPOSITE_STATUS_PASS, []) == COMPOSITE_STATUS_PASS
    assert effective_composite_status(COMPOSITE_STATUS_PASS, [{"action": "lint"}]) == COMPOSITE_STATUS_FAIL
