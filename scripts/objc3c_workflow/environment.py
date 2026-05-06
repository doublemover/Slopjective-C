"""Shared environment discovery for the objc3c workflow package."""

from __future__ import annotations

import shutil
from pathlib import Path


SCRIPT_ROOT = Path(__file__).resolve().parents[1]
ROOT = Path(__file__).resolve().parents[2]

PWSH = shutil.which("pwsh") or "pwsh"
NPX = shutil.which("npx.cmd") or shutil.which("npx") or "npx"

WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_COMMAND_TEXT = f"python -m {WORKFLOW_MODULE}"
WORKFLOW_RUNNER_SURFACE = WORKFLOW_MODULE
WORKFLOW_RUNNER_MODE = "objc3c-workflow-action-registry-v1"

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
