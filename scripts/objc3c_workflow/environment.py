"""Shared environment discovery for the objc3c workflow package."""

from __future__ import annotations

from .environment_commands import WORKFLOW_COMMAND_TEXT, WORKFLOW_PUBLIC_COMMAND
from .environment_markdown import MARKDOWN_GLOBS
from .environment_tools import NPX, PWSH
from .paths import ROOT, SCRIPT_ROOT
from .public_bridge_constants import (
    WORKFLOW_MODULE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)


__all__ = [
    "MARKDOWN_GLOBS",
    "NPX",
    "PWSH",
    "ROOT",
    "SCRIPT_ROOT",
    "WORKFLOW_COMMAND_TEXT",
    "WORKFLOW_MODULE",
    "WORKFLOW_PUBLIC_COMMAND",
    "WORKFLOW_RUNNER_MODE",
    "WORKFLOW_RUNNER_SURFACE",
]
