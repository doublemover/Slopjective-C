"""Host tool lookup helpers for required workflow environment constants."""

from __future__ import annotations

import shutil

from .environment_tool_registry import WorkflowToolSpec, workflow_tool_spec


WORKFLOW_TOOL_LOOKUP_OWNER = "objc3c-workflow-required-tool-lookup"


def resolve_required_tool(spec: WorkflowToolSpec) -> str:
    for candidate in spec.candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    candidates = ", ".join(spec.candidates)
    raise RuntimeError(
        f"required workflow tool not found for {spec.tool_id}: {candidates}"
    )


def required_workflow_tool(tool_id: str) -> str:
    return resolve_required_tool(workflow_tool_spec(tool_id))


__all__ = [
    "WORKFLOW_TOOL_LOOKUP_OWNER",
    "required_workflow_tool",
    "resolve_required_tool",
]
