from __future__ import annotations

from pathlib import Path

from scripts.objc3c_runtime_acceptance.case_result import CaseResult
from scripts.objc3c_runtime_acceptance.progress_snapshots import (
    build_final_progress_summary,
    progress_snapshot_status,
)
from scripts.objc3c_runtime_acceptance.result_normalization import (
    normalize_case_result,
)
from scripts.objc3c_runtime_acceptance.summary_owner_contracts import (
    ARTIFACT_EVIDENCE_OWNER_SURFACE,
    FINAL_STATUS_OWNER_SURFACE,
    PROGRESS_OWNER_SURFACE,
    REPORT_ASSEMBLY_OWNER_SURFACE,
    REPORTING_OWNER_SURFACE,
    RESULT_NORMALIZATION_OWNER_SURFACE,
    RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID,
    SUMMARY_OWNER_SURFACE,
    build_reporting_owner_contract,
)


ROOT = Path(__file__).resolve().parents[2]
RUNTIME_ACCEPTANCE_ROOT = ROOT / "scripts" / "objc3c_runtime_acceptance"
FORBIDDEN_REPORTING_WORDS = (
    "report-only",
    "report only",
    "fallback",
    "migration",
    "compat",
    "compatibility",
    "legacy",
)


def test_runtime_acceptance_reporting_owner_contract_covers_surfaces() -> None:
    contract = build_reporting_owner_contract()

    assert contract["contract_id"] == RUNTIME_ACCEPTANCE_REPORTING_OWNER_CONTRACT_ID
    assert contract["owner_surface"] == REPORTING_OWNER_SURFACE
    assert contract["summary_owner_contract"]["owner_surface"] == SUMMARY_OWNER_SURFACE
    assert (
        contract["result_normalization_owner_contract"]["owner_surface"]
        == RESULT_NORMALIZATION_OWNER_SURFACE
    )
    assert contract["progress_owner_contract"]["owner_surface"] == PROGRESS_OWNER_SURFACE
    assert (
        contract["report_assembly_owner_contract"]["owner_surface"]
        == REPORT_ASSEMBLY_OWNER_SURFACE
    )
    assert (
        contract["artifact_evidence_owner_contract"]["owner_surface"]
        == ARTIFACT_EVIDENCE_OWNER_SURFACE
    )
    assert (
        contract["final_status_owner_contract"]["owner_surface"]
        == FINAL_STATUS_OWNER_SURFACE
    )


def test_case_result_normalization_publishes_owner_and_status() -> None:
    normalized = normalize_case_result(
        CaseResult(
            case_id="runtime-owner-case",
            probe="runtime-probe",
            fixture=None,
            claim_class="linked-runtime-probe",
            passed=True,
            summary={"evidence": "owned"},
        )
    )

    assert normalized["owner_surface"] == RESULT_NORMALIZATION_OWNER_SURFACE
    assert normalized["status"] == "PASS"
    assert normalized["passed"] is True
    assert normalized["summary"] == {"evidence": "owned"}


def test_progress_status_decision_is_owner_explicit() -> None:
    assert progress_snapshot_status({"error": "boom"}) == "FAIL"
    assert progress_snapshot_status({"label": "case"}) == "RUNNING"

    final_summary = build_final_progress_summary(
        {
            "completed_case_count": 2,
            "total_case_count": 2,
            "current_case": {"label": "case"},
            "current_command": {"command": "cmd"},
        }
    )

    assert final_summary["status"] == "PASS"
    assert final_summary["status_decision"]["owner_surface"] == (
        FINAL_STATUS_OWNER_SURFACE
    )
    assert final_summary["status_decision"]["all_cases_completed"] is True
    assert final_summary["current_case"] is None
    assert final_summary["current_command"] is None


def test_runtime_acceptance_reporting_lane_avoids_retired_language() -> None:
    reporting_files = [
        *RUNTIME_ACCEPTANCE_ROOT.glob("summary*.py"),
        *RUNTIME_ACCEPTANCE_ROOT.glob("progress*.py"),
        RUNTIME_ACCEPTANCE_ROOT / "report_assembly.py",
        RUNTIME_ACCEPTANCE_ROOT / "reports.py",
        RUNTIME_ACCEPTANCE_ROOT / "result_normalization.py",
        RUNTIME_ACCEPTANCE_ROOT / "case_result.py",
    ]

    for path in reporting_files:
        text = path.read_text(encoding="utf-8").lower()
        for forbidden in FORBIDDEN_REPORTING_WORDS:
            assert forbidden not in text, f"{forbidden} leaked through {path}"
