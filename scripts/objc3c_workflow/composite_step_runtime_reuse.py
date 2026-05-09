"""Runtime acceptance report reuse for composite workflow steps."""

from __future__ import annotations

import os
from collections.abc import Sequence
from time import perf_counter


def runtime_acceptance_reuse_step(
    action: str,
    command: Sequence[str],
    started_at: float,
) -> dict[str, object] | None:
    if (
        action.startswith("test-runtime-acceptance")
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return {
            "action": action,
            "command": [str(token) for token in command],
            "exit_code": 0,
            "report_paths": ["tmp/reports/runtime/acceptance/summary.json"],
            "report_reused": True,
            "report_reuse_source": "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN",
            "duration_seconds": round(perf_counter() - started_at, 6),
        }
    return None
