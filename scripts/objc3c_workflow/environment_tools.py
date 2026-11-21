"""Registry names resolved only when a workflow command executes."""

from __future__ import annotations

from .environment_tool_lookup import required_workflow_tool


PWSH = "pwsh"
NPX = "npx"


__all__ = ["NPX", "PWSH", "required_workflow_tool"]
