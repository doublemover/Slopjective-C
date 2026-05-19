"""Developer-tooling workflow step execution."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.subprocesses import python_script_command, run_timed
from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .constants import FORMATTER_DEBUG_SURFACE_PY, ROOT, WORKSPACE_INTEGRATION_PY


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
        run_step("check-formatter-debug-surface", python_script_command(FORMATTER_DEBUG_SURFACE_PY)),
        run_step("check-workspace-editor-debug-surface", python_script_command(WORKSPACE_INTEGRATION_PY)),
        run_step("inspect-capability-explorer", public_workflow_command("inspect-capability-explorer")),
        run_step("benchmark-runtime-inspector", public_workflow_command("benchmark-runtime-inspector")),
        run_step("trace-compile-stages", public_workflow_command("trace-compile-stages")),
    ]
