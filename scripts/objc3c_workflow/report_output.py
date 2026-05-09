"""JSON stdout emission for workflow reports."""

from __future__ import annotations

import sys

from .report_rendering import render_report_json


def emit_json(payload: object) -> int:
    sys.stdout.write(render_report_json(payload))
    return 0


__all__ = ["emit_json"]
