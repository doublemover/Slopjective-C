"""Public API for validation surface inventory generation."""

from __future__ import annotations

from .classification import STATIC_GUARD_OVERRIDES, classify_check_py, referenced_by
from .cli import build_parser, main, parse_args, run
from .discovery import (
    acceptance_harness_catalog,
    all_check_py_files,
    all_test_check_py_files,
    all_validation_ps1_files,
    package_scripts,
    run_json,
    workflow_backend_text,
)
from .filtering import retained_static_guards, unreferenced_check_surfaces
from .model import build_inventory_entries, build_report, migration_map, non_goals
from .paths import ACCEPTANCE_HARNESS, CHECK_ROOTS, JSON_OUT, MD_OUT, PACKAGE_JSON, REPORT_DIR, ROOT, TASK_HYGIENE_GATE
from .public import write_report
from .rendering import render_markdown

__all__ = [
    "ACCEPTANCE_HARNESS",
    "CHECK_ROOTS",
    "JSON_OUT",
    "MD_OUT",
    "PACKAGE_JSON",
    "REPORT_DIR",
    "ROOT",
    "STATIC_GUARD_OVERRIDES",
    "TASK_HYGIENE_GATE",
    "acceptance_harness_catalog",
    "all_check_py_files",
    "all_test_check_py_files",
    "all_validation_ps1_files",
    "build_inventory_entries",
    "build_parser",
    "build_report",
    "classify_check_py",
    "main",
    "migration_map",
    "non_goals",
    "package_scripts",
    "parse_args",
    "referenced_by",
    "render_markdown",
    "retained_static_guards",
    "run",
    "run_json",
    "unreferenced_check_surfaces",
    "workflow_backend_text",
    "write_report",
]
