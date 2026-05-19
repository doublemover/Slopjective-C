"""Dashboard section shaping for validation timing reports."""

from __future__ import annotations

from pathlib import Path

from .validation_timing_numbers import safe_float
from .validation_timing_report_io import relative_path_or_none
from .validation_timing_report_summaries import (
    summarize_execution_replay_report,
    summarize_execution_smoke_report,
    summarize_runtime_acceptance_report,
)


def dashboard_section_from_report(
    label: str, path: Path | None, payload: dict[str, object] | None
) -> dict[str, object]:
    if payload is None:
        return {
            "label": label,
            "report_path": relative_path_or_none(path),
            "status": "MISSING",
            "report_owner": "validation_timing_report_io",
            "source_owner": "validation_timing_dashboard_sections",
        }
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    section: dict[str, object] = {
        "label": label,
        "report_path": relative_path_or_none(path),
        "status": payload.get("status", "UNKNOWN"),
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "report_owner": "validation_timing_report_io",
        "source_owner": "validation_timing_dashboard_sections",
    }
    if "case_count" in payload or "default_compile_backend" in payload:
        section.update(
            summarize_runtime_acceptance_report(
                relative_path_or_none(path) or "",
                payload,
                report_reused=False,
            )
        )
    elif "proof_run_id" in payload:
        section.update(
            summarize_execution_replay_report(relative_path_or_none(path) or "", payload)
        )
    elif "compile_command" in payload and "results" in payload:
        section.update(
            summarize_execution_smoke_report(relative_path_or_none(path) or "", payload)
        )
    elif "child_timing" in payload:
        section["child_timing"] = payload.get("child_timing")
        section["slowest_steps"] = timing_payload.get("slowest_steps", [])
        section["estimated_no_skip_seconds"] = timing_payload.get(
            "estimated_no_skip_seconds"
        )
    return section
