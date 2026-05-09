"""Host tool discovery for objc3c workflow actions."""

from __future__ import annotations

from .environment_tool_lookup import first_available_tool


PWSH = first_available_tool("pwsh", fallback="pwsh")
NPX = first_available_tool("npx.cmd", "npx", fallback="npx")


__all__ = ["NPX", "PWSH", "first_available_tool"]
