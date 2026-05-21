"""Paths used by the developer-tooling integration check."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FORMATTER_DEBUG_SURFACE_PY = ROOT / "scripts" / "check_developer_tooling_formatter_debug_surface.py"
FORMATTER_REWRITE_SURFACE_PY = ROOT / "scripts" / "check_developer_tooling_formatter_rewrite_surface.py"
DIAGNOSTIC_QUALITY_PY = ROOT / "scripts" / "check_developer_tooling_diagnostic_quality.py"
EDITOR_TOOLING_SOURCE_TRUTH_PY = ROOT / "scripts" / "check_developer_tooling_editor_source_truth.py"
PRODUCT_WORKFLOW_SOURCE_TRUTH_PY = ROOT / "scripts" / "check_developer_tooling_product_workflow_source_truth.py"
WORKSPACE_INTEGRATION_PY = ROOT / "scripts" / "check_developer_tooling_workspace_integration.py"
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "developer-tooling" / "integration-summary.json"
EDITOR_SURFACE_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface" / "hello-3bb3df22f2ea" / "editor-surface.json"
FORMATTER_DEBUG_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter-debug" / "formatter_debug_summary.json"
FORMATTER_REWRITE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter-rewrite" / "formatter_rewrite_summary.json"
DIAGNOSTIC_QUALITY_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "diagnostic-quality" / "diagnostic_quality_summary.json"
EDITOR_TOOLING_SOURCE_TRUTH_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-source-truth-summary.json"
PRODUCT_WORKFLOW_SOURCE_TRUTH_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "product-workflow-source-truth-summary.json"
WORKSPACE_INTEGRATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "workspace-integration-summary.json"
