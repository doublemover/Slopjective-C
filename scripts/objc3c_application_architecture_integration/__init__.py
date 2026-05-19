"""Importable application architecture integration package."""

from __future__ import annotations

from .cli import main, run_integration_step
from .contracts import (
    CANONICAL_SUMMARY_PATH,
    CANONICAL_WORKSPACE_PY,
    CHILD_SUMMARY_PATHS,
    INTEGRATION_CONTRACT_ID,
    REPORT_PATH,
    ROOT,
    RUNNER_PATH,
    SHOWCASE_INTEGRATION_PY,
    SHOWCASE_SUMMARY_PATH,
    STDLIB_PROGRAM_INTEGRATION_PY,
    STDLIB_PROGRAM_SUMMARY_PATH,
    TEMPLATE_HARNESS_PY,
    TEMPLATE_SUMMARY_PATH,
    WORKFLOW_ACTIONS,
)
from .execution import expect, require_executed_summary, summary_passes
from .rendering import render_failures, render_summary_path, write_integration_summary
from .summary import build_integration_payload, build_step_result

__all__ = [
    "CANONICAL_SUMMARY_PATH",
    "CANONICAL_WORKSPACE_PY",
    "CHILD_SUMMARY_PATHS",
    "INTEGRATION_CONTRACT_ID",
    "REPORT_PATH",
    "ROOT",
    "RUNNER_PATH",
    "SHOWCASE_INTEGRATION_PY",
    "SHOWCASE_SUMMARY_PATH",
    "STDLIB_PROGRAM_INTEGRATION_PY",
    "STDLIB_PROGRAM_SUMMARY_PATH",
    "TEMPLATE_HARNESS_PY",
    "TEMPLATE_SUMMARY_PATH",
    "WORKFLOW_ACTIONS",
    "build_integration_payload",
    "build_step_result",
    "expect",
    "main",
    "render_failures",
    "render_summary_path",
    "require_executed_summary",
    "run_integration_step",
    "summary_passes",
    "write_integration_summary",
]
