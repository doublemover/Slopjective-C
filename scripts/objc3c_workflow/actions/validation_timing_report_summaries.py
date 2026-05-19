"""Summary payload builders for validation timing report families."""

from __future__ import annotations

from .validation_timing_numbers import safe_float


def classify_runtime_command(command: object) -> str:
    text = str(command).replace("\\", "/").lower()
    if "objc3c_native_compile.ps1" in text:
        return "wrapper"
    if "objc3c-native" in text:
        return "native"
    if "clang++" in text:
        return "clang++"
    if ".exe" in text:
        return "probe"
    return "other"


def summarize_runtime_acceptance_report(
    path: str, payload: dict[str, object], *, report_reused: bool
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    command_timings = timing_payload.get("command_timings", [])
    command_groups: dict[str, dict[str, object]] = {}
    if isinstance(command_timings, list):
        for entry in command_timings:
            if not isinstance(entry, dict):
                continue
            group = classify_runtime_command(entry.get("command", ""))
            bucket = command_groups.setdefault(
                group,
                {"count": 0, "duration_seconds": 0.0},
            )
            bucket["count"] = int(bucket["count"]) + 1
            bucket["duration_seconds"] = round(
                safe_float(bucket["duration_seconds"])
                + safe_float(entry.get("duration_seconds", 0.0)),
                6,
            )
    return {
        "report_path": path,
        "report_reused": report_reused,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "case_count": payload.get("case_count")
        or timing_payload.get("total_case_count"),
        "completed_case_count": timing_payload.get("completed_case_count"),
        "command_count": len(command_timings)
        if isinstance(command_timings, list)
        else None,
        "default_compile_backend": payload.get("default_compile_backend"),
        "direct_compile_backend": payload.get("direct_compile_backend"),
        "wrapper_compile_backend": payload.get("wrapper_compile_backend"),
        "command_groups": command_groups,
        "slowest_cases": timing_payload.get("slowest_cases", []),
        "slowest_commands": timing_payload.get("slowest_commands", []),
    }


def summarize_execution_smoke_report(
    path: str, payload: dict[str, object]
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    selection = payload.get("selection", {})
    selection_payload = selection if isinstance(selection, dict) else {}
    return {
        "report_path": path,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "status": payload.get("status"),
        "total": payload.get("total"),
        "passed": payload.get("passed"),
        "failed": payload.get("failed"),
        "selection": selection_payload,
        "stage_totals": timing_payload.get("stage_totals", {}),
        "slowest_fixtures": timing_payload.get("slowest_fixtures", []),
    }


def summarize_execution_replay_report(
    path: str, payload: dict[str, object]
) -> dict[str, object]:
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    return {
        "report_path": path,
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
        "status": payload.get("status"),
        "proof_run_id": payload.get("proof_run_id"),
        "selection": payload.get("selection", {}),
        "stage_totals": timing_payload.get("stage_totals", {}),
        "slowest_cases": timing_payload.get("slowest_cases", []),
    }
