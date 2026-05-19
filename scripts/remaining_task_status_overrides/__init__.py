"""Remaining-task status override application internals."""

from __future__ import annotations

from .applying import apply_overrides
from .catalog import load_catalog, validate_catalog_status_invariants
from .cli import build_parser, main
from .constants import ALLOWED_STATUSES
from .json_files import read_json, write_catalog
from .models import OverrideEntry
from .override_loading import load_overrides, parse_override_entry
from .summary import render_summary
from .text import normalize_status, normalize_text, task_row_label


__all__ = [
    "ALLOWED_STATUSES",
    "OverrideEntry",
    "apply_overrides",
    "build_parser",
    "load_catalog",
    "load_overrides",
    "main",
    "normalize_status",
    "normalize_text",
    "parse_override_entry",
    "read_json",
    "render_summary",
    "task_row_label",
    "validate_catalog_status_invariants",
    "write_catalog",
]
