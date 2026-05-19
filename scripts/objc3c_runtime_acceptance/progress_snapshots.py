"""Runtime acceptance progress snapshot builders."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from .progress_format import repo_display_path
from .progress_format import round_seconds
from .summary_owner_contracts import FINAL_STATUS_OWNER_CONTRACT
from .summary_owner_contracts import PROGRESS_OWNER_CONTRACT


def progress_snapshot_status(current_case: dict[str, Any] | None) -> str:
    if current_case and current_case.get("error"):
        return "FAIL"
    return "RUNNING"


def build_progress_write_overhead(
    *,
    write_count: int,
    total_seconds: float,
    max_seconds: float,
    elapsed_seconds: float,
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.runtime.acceptance.progress.write.overhead.v1",
        "write_count": write_count,
        "total_seconds": round_seconds(total_seconds),
        "max_seconds": round_seconds(max_seconds),
        "overhead_percent_of_elapsed": round_seconds(
            (total_seconds / max(elapsed_seconds, 0.000001)) * 100.0
        ),
        "model": (
            "console progress stays immediate; progress.json remains the "
            "durable current-state snapshot while report-write overhead is measured"
        ),
    }


def build_progress_snapshot(
    *,
    run_id: str,
    run_dir: Path,
    progress_path: Path,
    elapsed_seconds: float,
    total_cases: int,
    current_case: dict[str, Any] | None,
    current_command: dict[str, Any] | None,
    last_completed_case: dict[str, Any] | None,
    case_timings: list[dict[str, Any]],
    command_timings: list[dict[str, Any]],
    progress_write_count: int,
    progress_write_seconds: float,
    progress_write_max_seconds: float,
) -> dict[str, Any]:
    return {
        "contract_id": PROGRESS_OWNER_CONTRACT.contract_id,
        "owner_surface": PROGRESS_OWNER_CONTRACT.owner_surface,
        "status": progress_snapshot_status(current_case),
        "run_id": run_id,
        "run_dir": repo_display_path(run_dir),
        "progress_path": repo_display_path(progress_path),
        "elapsed_seconds": elapsed_seconds,
        "total_case_count": total_cases,
        "completed_case_count": len(case_timings),
        "current_case": current_case,
        "current_command": current_command,
        "last_completed_case": last_completed_case,
        "status_decision": {
            "contract_id": FINAL_STATUS_OWNER_CONTRACT.contract_id,
            "owner_surface": FINAL_STATUS_OWNER_CONTRACT.owner_surface,
            "failed_current_case": bool(current_case and current_case.get("error")),
        },
        "case_timings": case_timings,
        "command_timings": command_timings,
        "progress_report_write_overhead": build_progress_write_overhead(
            write_count=progress_write_count,
            total_seconds=progress_write_seconds,
            max_seconds=progress_write_max_seconds,
            elapsed_seconds=elapsed_seconds,
        ),
        "slowest_cases": sorted(
            case_timings,
            key=lambda entry: float(entry.get("duration_seconds", 0.0)),
            reverse=True,
        )[:10],
        "slowest_commands": sorted(
            command_timings,
            key=lambda entry: float(entry.get("duration_seconds", 0.0)),
            reverse=True,
        )[:10],
    }


def build_final_progress_summary(snapshot: dict[str, Any]) -> dict[str, Any]:
    summary = dict(snapshot)
    all_cases_completed = (
        int(summary.get("completed_case_count", 0))
        == int(summary.get("total_case_count", 0))
    )
    summary["status"] = "PASS" if all_cases_completed else "FAIL"
    summary["status_decision"] = {
        "contract_id": FINAL_STATUS_OWNER_CONTRACT.contract_id,
        "owner_surface": FINAL_STATUS_OWNER_CONTRACT.owner_surface,
        "all_cases_completed": all_cases_completed,
    }
    summary["current_case"] = None
    summary["current_command"] = None
    return summary


__all__ = [
    "build_final_progress_summary",
    "build_progress_snapshot",
    "build_progress_write_overhead",
    "progress_snapshot_status",
]
