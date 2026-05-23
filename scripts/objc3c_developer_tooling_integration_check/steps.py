"""Developer-tooling workflow step execution."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.subprocesses import python_script_command, run_timed
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .constants import (
    DIAGNOSTIC_QUALITY_PY,
    EDITOR_TOOLING_SOURCE_TRUTH_PY,
    FORMATTER_DEBUG_SURFACE_PY,
    FORMATTER_REWRITE_SURFACE_PY,
    PRODUCT_WORKFLOW_SOURCE_TRUTH_PY,
    ROOT,
    WORKSPACE_INTEGRATION_PY,
)


def run_step(name: str, command: list[str]) -> dict[str, Any]:
    completed = run_timed(command, cwd=ROOT, echo=True)
    return {
        "name": name,
        "command": command,
        "exit_code": completed.returncode,
        "duration_ms": completed.duration_ms,
    }


def run_developer_tooling_steps() -> list[dict[str, Any]]:
    return [
        run_step("inspect-compile-observability", public_workflow_command("inspect-compile-observability")),
        run_step("inspect-runtime-inspector", public_workflow_command("inspect-runtime-inspector")),
        run_step("inspect-editor-tooling", public_workflow_command("inspect-editor-tooling")),
        run_step("inspect-source-graph", public_workflow_command("inspect-source-graph")),
        run_step("check-formatter-debug-surface", python_script_command(FORMATTER_DEBUG_SURFACE_PY)),
        run_step("check-formatter-rewrite-surface", python_script_command(FORMATTER_REWRITE_SURFACE_PY)),
        run_step("check-diagnostic-quality", python_script_command(DIAGNOSTIC_QUALITY_PY)),
        run_step("check-editor-tooling-source-truth", python_script_command(EDITOR_TOOLING_SOURCE_TRUTH_PY)),
        run_step("check-product-workflow-source-truth", python_script_command(PRODUCT_WORKFLOW_SOURCE_TRUTH_PY)),
        run_step("check-workspace-editor-debug-surface", python_script_command(WORKSPACE_INTEGRATION_PY)),
        run_step("inspect-capability-explorer", public_workflow_command("inspect-capability-explorer")),
        run_step("benchmark-runtime-inspector", public_workflow_command("benchmark-runtime-inspector")),
        run_step("trace-compile-stages", public_workflow_command("trace-compile-stages")),
        run_step("trace-runtime-debug", public_workflow_command("trace-runtime-debug")),
    ]
