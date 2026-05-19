#!/usr/bin/env python3

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from execution_microtasks import DEFAULT_CATALOG_JSON
from execution_microtasks import ROOT
from execution_microtasks import Issue
from execution_microtasks import build_parser
from execution_microtasks import load_issues
from execution_microtasks import main
from execution_microtasks import normalize_inline_text
from execution_microtasks import normalize_line_endings
from execution_microtasks import parse_catalog_task_identifier
from execution_microtasks import parse_generated_on
from execution_microtasks import parse_issue_number
from execution_microtasks import parse_issue_title
from execution_microtasks import parse_labels
from execution_microtasks import parse_non_negative_int
from execution_microtasks import render_markdown
from execution_microtasks import resolve_generated_on
from execution_microtasks import source_date_epoch_to_date
from execution_microtasks import validate_catalog_status_integrity
from execution_microtasks import write_stdout

__all__ = [
    "DEFAULT_CATALOG_JSON",
    "ROOT",
    "Issue",
    "build_parser",
    "load_issues",
    "main",
    "normalize_inline_text",
    "normalize_line_endings",
    "parse_catalog_task_identifier",
    "parse_generated_on",
    "parse_issue_number",
    "parse_issue_title",
    "parse_labels",
    "parse_non_negative_int",
    "render_markdown",
    "resolve_generated_on",
    "source_date_epoch_to_date",
    "validate_catalog_status_integrity",
    "write_stdout",
]


if __name__ == "__main__":
    raise SystemExit(main())
