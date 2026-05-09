"""Runtime acceptance report reuse for composite workflow steps."""

from __future__ import annotations

import os
from collections.abc import Sequence

from .composite_step_payload import composite_step_payload


def runtime_acceptance_reuse_step(
    action: str,
    command: Sequence[str],
    started_at: float,
) -> dict[str, object] | None:
    if (
        action.startswith("test-runtime-acceptance")
        and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"
    ):
        return composite_step_payload(
            action=action,
            command=command,
            exit_code=0,
            report_paths=["tmp/reports/runtime/acceptance/summary.json"],
            started_at=started_at,
            extra={
                "report_reused": True,
                "report_reuse_source": "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN",
            },
        )
    return None
