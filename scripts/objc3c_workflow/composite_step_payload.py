"""Composite workflow step payload construction."""

from __future__ import annotations

from collections.abc import Sequence
from time import perf_counter


def step_duration(started_at: float) -> float:
    return round(perf_counter() - started_at, 6)


def composite_step_payload(
    *,
    action: str,
    command: Sequence[str],
    exit_code: int,
    report_paths: Sequence[str],
    started_at: float,
    extra: dict[str, object] | None = None,
) -> dict[str, object]:
    payload: dict[str, object] = {
        "action": action,
        "command": [str(token) for token in command],
        "exit_code": exit_code,
        "report_paths": list(report_paths),
        "duration_seconds": step_duration(started_at),
    }
    if extra:
        payload.update(extra)
    return payload


__all__ = ["composite_step_payload", "step_duration"]
