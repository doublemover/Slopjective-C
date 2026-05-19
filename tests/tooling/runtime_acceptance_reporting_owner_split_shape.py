from scripts.objc3c_runtime_acceptance.progress_snapshots import (
    build_final_progress_summary,
    progress_snapshot_status,
)
from scripts.objc3c_runtime_acceptance.result_normalization import (
    normalize_case_result,
)
from scripts.objc3c_runtime_acceptance.summary_owner_contracts import (
    FINAL_STATUS_OWNER_SURFACE,
    RESULT_NORMALIZATION_OWNER_SURFACE,
)

from runtime_acceptance_reporting_owner_split_support import (
    completed_progress_payload,
    owned_runtime_case_result,
)


def case_result_normalization_publishes_owner_and_status() -> None:
    normalized = normalize_case_result(owned_runtime_case_result())

    assert normalized["owner_surface"] == RESULT_NORMALIZATION_OWNER_SURFACE
    assert normalized["status"] == "PASS"
    assert normalized["passed"] is True
    assert normalized["summary"] == {"evidence": "owned"}


def progress_status_decision_is_owner_explicit() -> None:
    assert progress_snapshot_status({"error": "boom"}) == "FAIL"
    assert progress_snapshot_status({"label": "case"}) == "RUNNING"

    final_summary = build_final_progress_summary(completed_progress_payload())

    assert final_summary["status"] == "PASS"
    assert final_summary["status_decision"]["owner_surface"] == (
        FINAL_STATUS_OWNER_SURFACE
    )
    assert final_summary["status_decision"]["all_cases_completed"] is True
    assert final_summary["current_case"] is None
    assert final_summary["current_command"] is None
