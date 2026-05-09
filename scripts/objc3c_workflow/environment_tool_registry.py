"""Required host tool registry for objc3c workflow commands."""

from __future__ import annotations

from dataclasses import dataclass


WORKFLOW_TOOL_REGISTRY_OWNER = "objc3c-workflow-required-tool-registry"


@dataclass(frozen=True)
class WorkflowToolSpec:
    tool_id: str
    candidates: tuple[str, ...]
    owner: str = WORKFLOW_TOOL_REGISTRY_OWNER


WORKFLOW_TOOL_REGISTRY: dict[str, WorkflowToolSpec] = {
    "pwsh": WorkflowToolSpec("pwsh", ("pwsh",)),
    "npx": WorkflowToolSpec("npx", ("npx.cmd", "npx")),
}


def workflow_tool_spec(tool_id: str) -> WorkflowToolSpec:
    try:
        return WORKFLOW_TOOL_REGISTRY[tool_id]
    except KeyError as exc:
        raise KeyError(f"unknown required workflow tool: {tool_id}") from exc


__all__ = [
    "WORKFLOW_TOOL_REGISTRY",
    "WORKFLOW_TOOL_REGISTRY_OWNER",
    "WorkflowToolSpec",
    "workflow_tool_spec",
]
