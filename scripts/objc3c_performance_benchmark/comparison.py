from __future__ import annotations

import statistics
from typing import Any


def summarize_durations(durations: list[float], normalization_mode: str) -> dict[str, Any]:
    return {
        "sample_count": len(durations),
        "min_duration_ms": min(durations),
        "median_duration_ms": statistics.median(durations),
        "max_duration_ms": max(durations),
        "normalization_mode": normalization_mode,
        "availability_status": "local-measurement",
    }
