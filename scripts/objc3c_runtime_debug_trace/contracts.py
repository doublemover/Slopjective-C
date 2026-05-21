"""Contracts and canonical paths for runtime debug trace reports."""

from __future__ import annotations

from pathlib import Path
from typing import Final

from objc3c_tooling.paths import ROOT

RUNTIME_DEBUG_TRACE_CONTRACT_ID: Final[str] = "objc3c.runtime.debug.trace.v1"
RUNTIME_DEBUG_TRACE_SCHEMA_ID: Final[str] = "objc3c-runtime-debug-trace-v1"
RUNTIME_DEBUG_TRACE_SCHEMA_VERSION: Final[int] = 1
RUNTIME_DEBUG_TRACE_SCHEMA_PATH: Final[Path] = (
    ROOT / "schemas" / "objc3c-runtime-debug-trace-v1.schema.json"
)
PUBLIC_WORKFLOW_REPORT_ROOT: Final[Path] = (
    ROOT / "tmp" / "reports" / "objc3c-public-workflow"
)
RUNTIME_DEBUG_TRACE_SUMMARY_PATH: Final[Path] = (
    PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-debug-trace.json"
)
RUNTIME_INSPECTOR_REPORT_PATH: Final[Path] = (
    PUBLIC_WORKFLOW_REPORT_ROOT / "runtime-inspector.json"
)
COMPILE_STAGE_TRACE_REPORT_PATH: Final[Path] = (
    PUBLIC_WORKFLOW_REPORT_ROOT / "compile-stage-trace.json"
)
DEFAULT_TRACE_SOURCE: Final[Path] = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
)

RUNTIME_INSPECTOR_CONTRACT_ID: Final[str] = (
    "objc3c.runtime.metadata.object.inspection.harness.v1"
)
COMPILE_STAGE_TRACE_MODE: Final[str] = "objc3c-frontend-stage-trace-v1"
EDITOR_SURFACE_CONTRACT_ID: Final[str] = "objc3c.developer.tooling.editor.surface.v1"
DEBUG_MAP_CONTRACT_ID: Final[str] = "objc3c.developer.tooling.debug.map.surface.v1"

TRACE_ACTION: Final[str] = "trace-runtime-debug"
TRACE_REPORT_PATH_TEXT: Final[str] = (
    "tmp/reports/objc3c-public-workflow/runtime-debug-trace.json"
)
