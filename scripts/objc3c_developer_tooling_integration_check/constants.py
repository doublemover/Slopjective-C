"""Paths used by the developer-tooling integration check."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FORMATTER_DEBUG_SURFACE_PY = ROOT / "scripts" / "check_developer_tooling_formatter_debug_surface.py"
WORKSPACE_INTEGRATION_PY = ROOT / "scripts" / "check_developer_tooling_workspace_integration.py"
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "developer-tooling" / "integration-summary.json"
EDITOR_SURFACE_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface" / "hello-3bb3df22f2ea" / "editor-surface.json"
FORMATTER_DEBUG_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter-debug" / "formatter_debug_summary.json"
WORKSPACE_INTEGRATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "workspace-integration-summary.json"
