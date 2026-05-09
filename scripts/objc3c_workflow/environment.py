"""Shared environment discovery for the objc3c workflow package."""

from __future__ import annotations

import shutil

from .paths import ROOT, SCRIPT_ROOT
from .public_bridge import (
    WORKFLOW_MODULE,
    WORKFLOW_PUBLIC_COMMAND_PREFIX,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)


PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"

WORKFLOW_PUBLIC_COMMAND = WORKFLOW_PUBLIC_COMMAND_PREFIX
WORKFLOW_COMMAND_TEXT = WORKFLOW_PUBLIC_COMMAND

MARKDOWN_GLOBS = [
    "README.md",
    "CONTRIBUTING.md",
    "docs/**/*.md",
    "site/**/*.md",
    "spec/**/*.md",
    "showcase/**/*.md",
    "stdlib/**/*.md",
    "templates/**/*.md",
]
