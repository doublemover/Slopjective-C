"""Owned output policy for workflow report helpers."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import TextIO

REPORT_RENDERING_OWNER = "objc3c-workflow-report-rendering"
REPORT_STDOUT_OWNER = "objc3c-workflow-report-stdout"
REPORT_FILE_OWNER = "objc3c-workflow-report-file"
REPORT_SUCCESS_EXIT_CODE = 0


def report_stdout() -> TextIO:
    return sys.stdout


def report_success_exit_code() -> int:
    return REPORT_SUCCESS_EXIT_CODE


def report_file_path(path: Path) -> Path:
    return path


__all__ = [
    "REPORT_FILE_OWNER",
    "REPORT_RENDERING_OWNER",
    "REPORT_STDOUT_OWNER",
    "REPORT_SUCCESS_EXIT_CODE",
    "report_file_path",
    "report_stdout",
    "report_success_exit_code",
]
