"""Unknown workflow request diagnostics."""

from __future__ import annotations

from .request_error_output import emit_request_error
from .request_error_messages import (
    unknown_action_message,
    unknown_package_script_message,
)


def emit_unknown_action(action: str) -> int:
    return emit_request_error(unknown_action_message(action))


def emit_unknown_package_script(package_script: str) -> int:
    return emit_request_error(unknown_package_script_message(package_script))


__all__ = ["emit_unknown_action", "emit_unknown_package_script"]
