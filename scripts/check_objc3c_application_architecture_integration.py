#!/usr/bin/env python3
"""Validate template and canonical application integration on the live workflow."""

from __future__ import annotations

from objc3c_application_architecture_integration import (
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
    build_integration_payload,
    build_step_result,
    expect,
    main,
    render_failures,
    render_summary_path,
    require_executed_summary,
    run_integration_step,
    summary_passes,
    write_integration_summary,
)

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


if __name__ == "__main__":
    raise SystemExit(main())
