"""Subprocess execution for composite workflow steps."""

from __future__ import annotations

from collections.abc import Sequence

from objc3c_tooling.public_workflow_output import (
    extract_public_workflow_report_paths as extract_report_paths,
)
from objc3c_tooling.subprocesses import run_capture

from .composite_step_payload import composite_step_payload


def subprocess_step(
    action: str,
    command: Sequence[str],
    normalized: list[str],
    started_at: float,
) -> dict[str, object]:
    result = run_capture(command)
    return composite_step_payload(
        action=action,
        command=normalized,
        exit_code=result.returncode,
        report_paths=extract_report_paths(result.stdout),
        started_at=started_at,
    )
