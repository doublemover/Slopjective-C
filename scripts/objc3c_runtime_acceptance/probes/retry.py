"""Runtime probe retry policy and evidence state."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.progress_format import repo_display_path

RETRYABLE_PROBE_EXIT_CODES = {3221226356}
DEFAULT_PROBE_RETRIES = int(
    os.environ.get("OBJC3C_RUNTIME_ACCEPTANCE_PROBE_RETRIES", "1")
)
ACCEPTANCE_PROBE_RETRY_EVENTS: list[dict[str, Any]] = []


def record_successful_probe_retry(
    *,
    probe: Path,
    retry_count: int,
    attempts: list[dict[str, object]],
) -> None:
    ACCEPTANCE_PROBE_RETRY_EVENTS.append(
        {
            "probe": repo_display_path(probe),
            "retry_count": retry_count,
            "attempts": attempts,
            "model": (
                "retry is fail-closed and only masks transient process exits "
                "that are followed by a successful identical probe invocation"
            ),
        }
    )


__all__ = [
    "ACCEPTANCE_PROBE_RETRY_EVENTS",
    "DEFAULT_PROBE_RETRIES",
    "RETRYABLE_PROBE_EXIT_CODES",
    "record_successful_probe_retry",
]
