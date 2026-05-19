from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path
from objc3c_tooling.subprocesses import python_script_command

from .models import CommandResult
from .report_aggregation import build_summary_payload, summarize_command
from .report_constants import (
    BOOTSTRAP_JSON_FILENAME,
    BOOTSTRAP_JSON_LOG_FILENAME,
    BOOTSTRAP_MD_FILENAME,
    BOOTSTRAP_MD_LOG_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
)
from .report_rendering import bool_text, render_command_log, render_markdown_report
