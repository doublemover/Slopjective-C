"""Objective-C 2, Swift, and C++ migration analyzer workflow."""

from __future__ import annotations

from .analyzer import (
    REQUIRED_SURFACES,
    MigrationDiagnostic,
    MigrationReport,
    RewriteEdit,
    analyze_migration_input,
    apply_rewrite_plan,
    build_rewrite_workflow_report,
    default_analysis_report_path,
    default_rewrite_report_path,
    load_contract,
    load_migration_input,
    write_analysis_report,
    write_rewrite_report,
)

__all__ = [
    "REQUIRED_SURFACES",
    "MigrationDiagnostic",
    "MigrationReport",
    "RewriteEdit",
    "analyze_migration_input",
    "apply_rewrite_plan",
    "build_rewrite_workflow_report",
    "default_analysis_report_path",
    "default_rewrite_report_path",
    "load_contract",
    "load_migration_input",
    "write_analysis_report",
    "write_rewrite_report",
]
