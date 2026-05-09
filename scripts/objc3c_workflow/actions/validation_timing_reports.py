"""Validation timing report loading and summarization helpers."""

from __future__ import annotations

import json
from collections.abc import Sequence
from pathlib import Path

from ..environment import ROOT


def load_surface_from_report(
    steps: Sequence[dict[str, object]], surface_key: str
) -> dict[str, object] | None:
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            candidate = ROOT / raw_path
            if not candidate.is_file():
                continue
            try:
                payload = json.loads(candidate.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                continue
            surface = payload.get(surface_key)
            if isinstance(surface, dict):
                return surface
    return None


def safe_float(value: object, default: float = 0.0) -> float:
    if isinstance(value, (int, float)):
        return float(value)
    if isinstance(value, str):
        try:
            return float(value)
        except ValueError:
            return default
    return default


def load_json_report(raw_path: str) -> dict[str, object] | None:
    candidate = ROOT / raw_path
    if not candidate.is_file():
        return None
    try:
        payload = json.loads(candidate.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    if isinstance(payload, dict):
        return payload
    return None


def load_child_reports(
    steps: Sequence[dict[str, object]]
) -> list[dict[str, object]]:
    reports: list[dict[str, object]] = []
    for step in steps:
        report_paths = step.get("report_paths", [])
        if not isinstance(report_paths, list):
            continue
        for raw_path in report_paths:
            if not isinstance(raw_path, str):
                continue
            payload = load_json_report(raw_path)
            if payload is None:
                continue
            reports.append(
                {
                    "step_action": str(step.get("action", "")),
                    "path": raw_path,
                    "payload": payload,
                    "report_reused": bool(step.get("report_reused", False)),
                    "step_duration_seconds": safe_float(
                        step.get("duration_seconds", 0.0)
                    ),
                }
            )
    return reports


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


def latest_json_file(root: Path) -> Path | None:
    if root.is_file():
        return root
    if not root.exists():
        return None
    candidates = sorted(
        (candidate for candidate in root.rglob("*.json") if candidate.is_file()),
        key=lambda candidate: candidate.stat().st_mtime,
        reverse=True,
    )
    return candidates[0] if candidates else None


def relative_path_or_none(path: Path | None) -> str | None:
    if path is None:
        return None
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def load_latest_report_payload(path: Path | None) -> dict[str, object] | None:
    if path is None:
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    return payload if isinstance(payload, dict) else None


def dashboard_section_from_report(
    label: str, path: Path | None, payload: dict[str, object] | None
) -> dict[str, object]:
    if payload is None:
        return {
            "label": label,
            "report_path": relative_path_or_none(path),
            "status": "MISSING",
        }
    timing = payload.get("timing", {})
    timing_payload = timing if isinstance(timing, dict) else {}
    section: dict[str, object] = {
        "label": label,
        "report_path": relative_path_or_none(path),
        "status": payload.get("status", "UNKNOWN"),
        "elapsed_seconds": safe_float(timing_payload.get("elapsed_seconds", 0.0)),
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
