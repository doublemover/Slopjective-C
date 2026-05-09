"""Timing fields for composite public workflow reports."""

from __future__ import annotations


def composite_timing_payload(
    steps: list[dict[str, object]],
    child_timing: dict[str, object],
) -> dict[str, object]:
    return {
        "step_count": len(steps),
        "total_step_duration_seconds": round(
            sum(float(step.get("duration_seconds", 0.0)) for step in steps),
            6,
        ),
        "slowest_steps": sorted(
            [
                {
                    "action": str(step.get("action", "")),
                    "duration_seconds": float(step.get("duration_seconds", 0.0)),
                    "exit_code": int(step.get("exit_code", 0)),
                }
                for step in steps
            ],
            key=lambda entry: entry["duration_seconds"],
            reverse=True,
        )[:10],
        "estimated_no_skip_seconds": child_timing["estimated_no_skip_seconds"],
    }


__all__ = ["composite_timing_payload"]
