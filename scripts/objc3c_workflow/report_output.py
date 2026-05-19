"""JSON stdout emission for workflow reports."""

from __future__ import annotations

from .report_policy import report_stdout, report_success_exit_code
from .report_rendering import render_report_json


def emit_json(payload: object) -> int:
    report_stdout().write(render_report_json(payload))
    return report_success_exit_code()


__all__ = ["emit_json"]
