from __future__ import annotations

from pathlib import Path

from scripts.objc3c_runtime_acceptance.case_result import CaseResult

ROOT = Path(__file__).resolve().parents[2]
RUNTIME_ACCEPTANCE_ROOT = ROOT / "scripts" / "objc3c_runtime_acceptance"
FORBIDDEN_REPORTING_WORDS = (
    "evidence-log",
    "evidence log",
    "retired-route",
    "migration",
    "compat",
    "compatibility",
    "legacy",
)


def owned_runtime_case_result() -> CaseResult:
    return CaseResult(
        case_id="runtime-owner-case",
        probe="runtime-probe",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={"evidence": "owned"},
    )


def completed_progress_payload() -> dict:
    return {
        "completed_case_count": 2,
        "total_case_count": 2,
        "current_case": {"label": "case"},
        "current_command": {"command": "cmd"},
    }


def reporting_files() -> list[Path]:
    return [
        *RUNTIME_ACCEPTANCE_ROOT.glob("summary*.py"),
        *RUNTIME_ACCEPTANCE_ROOT.glob("progress*.py"),
        RUNTIME_ACCEPTANCE_ROOT / "report_assembly.py",
        RUNTIME_ACCEPTANCE_ROOT / "reports.py",
        RUNTIME_ACCEPTANCE_ROOT / "result_normalization.py",
        RUNTIME_ACCEPTANCE_ROOT / "case_result.py",
    ]
