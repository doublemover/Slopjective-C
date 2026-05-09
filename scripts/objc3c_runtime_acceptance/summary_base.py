"""Base runtime acceptance summary metadata."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.compile_backends import DEFAULT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import WRAPPER_COMPILE_BACKEND
from objc3c_runtime_acceptance.diagnostics import build_probe_retry_policy
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.paths import RUNTIME_LIB
from objc3c_runtime_acceptance.probes import ACCEPTANCE_PROBE_RETRY_EVENTS
from objc3c_runtime_acceptance.progress_format import repo_display_path
from objc3c_runtime_acceptance.progress_state import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.result_normalization import normalize_case_results
from objc3c_runtime_acceptance.runtime_artifact_registry import ACCEPTANCE_ARTIFACT_REGISTRY
from objc3c_runtime_acceptance.c_api import PUBLIC_RUNTIME_ABI_BOUNDARY
from objc3c_runtime_acceptance.surfaces import build_claim_boundary
from objc3c_runtime_acceptance.summary_owner_contracts import (
    ARTIFACT_EVIDENCE_OWNER_CONTRACT,
    FINAL_STATUS_OWNER_CONTRACT,
    RESULT_NORMALIZATION_OWNER_CONTRACT,
)


def runtime_acceptance_summary_status(results: list[CaseResult]) -> str:
    if all(result.passed for result in results):
        return "PASS"
    return "FAIL"


def build_base_summary_fields(
    *,
    args: Any,
    run_dir: Path,
    progress_path: Path,
    clangxx: str,
    results: list[CaseResult],
    acceptance_progress: RuntimeAcceptanceProgress,
    available_suites: dict[str, tuple[str, ...]],
) -> dict[str, Any]:
    return {
        "status": runtime_acceptance_summary_status(results),
        "status_decision": {
            "contract_id": FINAL_STATUS_OWNER_CONTRACT.contract_id,
            "owner_surface": FINAL_STATUS_OWNER_CONTRACT.owner_surface,
            "case_statuses": {
                result.case_id: result.status for result in results
            },
        },
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "selected_suite": args.suite,
        "selected_cases": list(args.cases),
        "available_suites": {
            suite: list(cases) if cases else "all"
            for suite, cases in available_suites.items()
        },
        "default_compile_backend": DEFAULT_COMPILE_BACKEND,
        "direct_compile_backend": DIRECT_COMPILE_BACKEND,
        "wrapper_compile_backend": WRAPPER_COMPILE_BACKEND,
        "probe_retry_policy": build_probe_retry_policy(),
        "probe_retry_events": ACCEPTANCE_PROBE_RETRY_EVENTS,
        "progress_report_path": repo_display_path(progress_path),
        "timing": acceptance_progress.final_summary(),
        "artifact_evidence_owner_contract": ARTIFACT_EVIDENCE_OWNER_CONTRACT.payload(),
        "artifact_registry": ACCEPTANCE_ARTIFACT_REGISTRY.summary(),
        "result_normalization_owner_contract": (
            RESULT_NORMALIZATION_OWNER_CONTRACT.payload()
        ),
        "cases": normalize_case_results(results),
        "claim_boundary": build_claim_boundary(PUBLIC_RUNTIME_ABI_BOUNDARY),
    }


__all__ = [
    "build_base_summary_fields",
    "runtime_acceptance_summary_status",
]
