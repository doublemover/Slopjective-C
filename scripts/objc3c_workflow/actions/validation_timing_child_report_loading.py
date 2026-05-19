"""Child report collection for composite validation timing payloads."""

from __future__ import annotations

from collections.abc import Sequence

from .validation_timing_numbers import safe_float
from .validation_timing_report_io import iter_step_report_paths, load_json_report


def load_child_reports(
    steps: Sequence[dict[str, object]]
) -> list[dict[str, object]]:
    reports: list[dict[str, object]] = []
    for step, raw_path in iter_step_report_paths(steps):
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
