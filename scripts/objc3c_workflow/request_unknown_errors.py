"""Unknown workflow request diagnostics."""

from __future__ import annotations

from .request_error_output import emit_request_error


def emit_unknown_action(action: str) -> int:
    return emit_request_error(f"unknown action: {action}")


def emit_unknown_package_script(package_script: str) -> int:
    return emit_request_error(f"unknown package script: {package_script}")


__all__ = ["emit_unknown_action", "emit_unknown_package_script"]
