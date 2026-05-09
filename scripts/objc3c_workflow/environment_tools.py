"""Host tool discovery for objc3c workflow actions."""

from __future__ import annotations

from .environment_tool_lookup import required_workflow_tool


PWSH = required_workflow_tool("pwsh")
NPX = required_workflow_tool("npx")


__all__ = ["NPX", "PWSH", "required_workflow_tool"]
