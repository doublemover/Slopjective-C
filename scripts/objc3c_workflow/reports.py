"""Workflow report helper facade."""

from __future__ import annotations

from .report_files import write_json_report
from .report_output import emit_json


__all__ = ["emit_json", "write_json_report"]
