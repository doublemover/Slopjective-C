"""Activation preflight report and summary builders."""

from __future__ import annotations

from scripts.activation_preflight.report_command_logs import (
    render_command_log,
    render_spec_lint_log,
)
from scripts.activation_preflight.report_constants import (
    ACTIVATION_JSON_FILENAME,
    ACTIVATION_MD_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SNAPSHOT_CAPTURE_LOG_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
)
from scripts.activation_preflight.report_markdown import render_markdown_report
from scripts.activation_preflight.report_summary import build_summary_payload
from scripts.activation_preflight.report_summary import summarize_command

__all__ = [
    "ACTIVATION_JSON_FILENAME",
    "ACTIVATION_MD_FILENAME",
    "OPEN_BLOCKERS_REFRESH_LOG_FILENAME",
    "REPORT_MD_FILENAME",
    "SNAPSHOT_CAPTURE_LOG_FILENAME",
    "SPEC_LINT_LOG_FILENAME",
    "SUMMARY_JSON_FILENAME",
    "build_summary_payload",
    "render_command_log",
    "render_markdown_report",
    "render_spec_lint_log",
    "summarize_command",
]
