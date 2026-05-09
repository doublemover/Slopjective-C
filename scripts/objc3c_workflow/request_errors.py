"""Request diagnostic emitters for the workflow CLI."""

from __future__ import annotations

import sys

from .argument_requests import WorkflowUsageError


def emit_usage_error(exc: WorkflowUsageError) -> int:
    print(exc.message, file=sys.stderr)
    return exc.exit_code


def emit_unknown_action(action: str) -> int:
    print(f"unknown action: {action}", file=sys.stderr)
    return 2


def emit_unknown_package_script(package_script: str) -> int:
    print(f"unknown package script: {package_script}", file=sys.stderr)
    return 2
