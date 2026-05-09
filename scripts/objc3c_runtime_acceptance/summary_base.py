"""Base runtime acceptance summary metadata."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.diagnostics import build_probe_retry_policy
from objc3c_runtime_acceptance.native_build import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
    DEFAULT_COMPILE_BACKEND,
    DIRECT_COMPILE_BACKEND,
    ROOT,
    RUNTIME_LIB,
    WRAPPER_COMPILE_BACKEND,
)
from objc3c_runtime_acceptance.probes import ACCEPTANCE_PROBE_RETRY_EVENTS
from objc3c_runtime_acceptance.progress import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.progress import repo_display_path
from objc3c_runtime_acceptance.runtime_contracts import PUBLIC_RUNTIME_ABI_BOUNDARY
from objc3c_runtime_acceptance.surfaces import build_claim_boundary


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
        "status": "PASS",
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
        "artifact_registry": ACCEPTANCE_ARTIFACT_REGISTRY.summary(),
        "cases": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "claim_class": result.claim_class,
                "passed": result.passed,
                "summary": result.summary,
            }
            for result in results
        ],
        "claim_boundary": build_claim_boundary(PUBLIC_RUNTIME_ABI_BOUNDARY),
    }


__all__ = ["build_base_summary_fields"]
