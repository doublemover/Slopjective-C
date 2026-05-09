"""Canonical public workflow bridge constants."""

from __future__ import annotations


WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_BRIDGE_SCRIPT = "objc3c"
WORKFLOW_PUBLIC_COMMAND_PREFIX = f"npm run {WORKFLOW_BRIDGE_SCRIPT} --"
WORKFLOW_RUNNER_SURFACE = f"package.json scripts.{WORKFLOW_BRIDGE_SCRIPT} -> {WORKFLOW_MODULE}"
WORKFLOW_RUNNER_MODE = "objc3c-workflow-action-registry-v1"
PUBLIC_ENTRYPOINT_KIND = "single-npm-bridge"


__all__ = [
    "PUBLIC_ENTRYPOINT_KIND",
    "WORKFLOW_BRIDGE_SCRIPT",
    "WORKFLOW_MODULE",
    "WORKFLOW_PUBLIC_COMMAND_PREFIX",
    "WORKFLOW_RUNNER_MODE",
    "WORKFLOW_RUNNER_SURFACE",
]
