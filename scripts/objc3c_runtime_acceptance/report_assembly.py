"""Report assembly and persistence for runtime acceptance."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.progress_state import RuntimeAcceptanceProgress
from objc3c_runtime_acceptance.reports import write_json_report
from objc3c_runtime_acceptance.scenario_loading import RuntimeAcceptanceScenarios
from objc3c_runtime_acceptance.summary import build_runtime_acceptance_summary


def assemble_runtime_acceptance_summary(
    *,
    args: Any,
    run_dir: Path,
    report_path: Path,
    progress_path: Path,
    clangxx: str,
    results: list[CaseResult],
    acceptance_progress: RuntimeAcceptanceProgress,
    scenarios: RuntimeAcceptanceScenarios,
    available_suites: dict[str, tuple[str, ...]],
) -> dict[str, Any]:
    return build_runtime_acceptance_summary(
        args=args,
        run_dir=run_dir,
        report_path=report_path,
        progress_path=progress_path,
        clangxx=clangxx,
        results=results,
        acceptance_progress=acceptance_progress,
        domains=scenarios.domains,
        available_suites=available_suites,
    )


def persist_runtime_acceptance_reports(
    *,
    progress_path: Path,
    report_path: Path,
    acceptance_progress: RuntimeAcceptanceProgress,
    summary: dict[str, Any],
) -> None:
    write_json_report(progress_path, acceptance_progress.final_summary())
    write_json_report(report_path, summary)


__all__ = [
    "assemble_runtime_acceptance_summary",
    "persist_runtime_acceptance_reports",
]
