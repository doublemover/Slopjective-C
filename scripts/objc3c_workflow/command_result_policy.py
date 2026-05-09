"""Owned exit-code and diagnostic policy for workflow command results."""

from __future__ import annotations

import sys
from typing import TextIO

COMMAND_RESULT_POLICY_OWNER = "objc3c-workflow-command-result-policy"
COMMAND_RESULT_ERROR_STREAM_OWNER = "objc3c-workflow-command-result-stderr"
COMMAND_RESULT_ACCEPTED_EXIT_CODE = 0
COMMAND_RESULT_REJECTED_EXIT_CODE = 2


def command_result_error_stream() -> TextIO:
    return sys.stderr


def unknown_action_result_message(action: str) -> str:
    return f"unknown action: {action}"


def extra_arguments_result_message(action: str) -> str:
    return f"action does not accept extra arguments: {action}"


def emit_command_result_message(message: str) -> None:
    if message:
        print(message, file=command_result_error_stream())


__all__ = [
    "COMMAND_RESULT_ACCEPTED_EXIT_CODE",
    "COMMAND_RESULT_ERROR_STREAM_OWNER",
    "COMMAND_RESULT_POLICY_OWNER",
    "COMMAND_RESULT_REJECTED_EXIT_CODE",
    "command_result_error_stream",
    "emit_command_result_message",
    "extra_arguments_result_message",
    "unknown_action_result_message",
]
