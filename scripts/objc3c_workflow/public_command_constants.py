"""Canonical public command constants for objc3c workflow callers."""

from __future__ import annotations

from pathlib import Path

from .path_bootstrap import install_workflow_import_roots
from .public_bridge_constants import WORKFLOW_BRIDGE_SCRIPT, WORKFLOW_MODULE

install_workflow_import_roots()

WORKFLOW_DISPATCH_MODULE = "scripts.objc3c_workflow.action_dispatch"
WORKFLOW_PACKAGE_MANAGER = "npm"
WORKFLOW_NPM_ARGUMENT_SEPARATOR = "--"
WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS = (
    WORKFLOW_PACKAGE_MANAGER,
    "run",
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_NPM_ARGUMENT_SEPARATOR,
)
DEFAULT_DISPATCH_PATH = Path(__file__).resolve().parent / "action_dispatch.py"


__all__ = [
    "DEFAULT_DISPATCH_PATH",
    "WORKFLOW_DISPATCH_MODULE",
    "WORKFLOW_MODULE",
    "WORKFLOW_NPM_ARGUMENT_SEPARATOR",
    "WORKFLOW_PACKAGE_MANAGER",
    "WORKFLOW_PUBLIC_COMMAND_PREFIX_ARGS",
]
