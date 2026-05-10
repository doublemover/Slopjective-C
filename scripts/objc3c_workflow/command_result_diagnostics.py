"""Command-result diagnostic messages and stderr emission."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from typing import TextIO

COMMAND_RESULT_DIAGNOSTIC_OWNER = "objc3c-workflow-command-result-diagnostics"
COMMAND_RESULT_ERROR_STREAM_OWNER = "objc3c-workflow-command-result-stderr"


@dataclass(frozen=True)
class CommandResultDiagnostic:
    message: str
    stream_owner: str = COMMAND_RESULT_ERROR_STREAM_OWNER

    @property
    def should_emit(self) -> bool:
        return bool(self.message)


def command_result_error_stream() -> TextIO:
    return sys.stderr


def unknown_action_result_message(action: str) -> str:
    return f"unknown action: {action}"


def extra_arguments_result_message(action: str) -> str:
    return f"action does not accept extra arguments: {action}"


def command_result_diagnostic(message: str) -> CommandResultDiagnostic:
    return CommandResultDiagnostic(message=message)


def emit_command_result_diagnostic(diagnostic: CommandResultDiagnostic) -> None:
    if diagnostic.should_emit:
        print(diagnostic.message, file=command_result_error_stream())


def emit_command_result_message(message: str) -> None:
    emit_command_result_diagnostic(command_result_diagnostic(message))


def command_result_diagnostics_contract_payload() -> dict[str, object]:
    return {
        "contract_id": COMMAND_RESULT_DIAGNOSTIC_OWNER,
        "owner_surface": "scripts/objc3c_workflow/command_result_diagnostics.py",
        "stream_owner": COMMAND_RESULT_ERROR_STREAM_OWNER,
        "emits_empty_messages": False,
        "unknown_action_message_shape": unknown_action_result_message("<action>"),
        "extra_arguments_message_shape": extra_arguments_result_message("<action>"),
    }


__all__ = [
    "COMMAND_RESULT_DIAGNOSTIC_OWNER",
    "COMMAND_RESULT_ERROR_STREAM_OWNER",
    "CommandResultDiagnostic",
    "command_result_diagnostic",
    "command_result_diagnostics_contract_payload",
    "command_result_error_stream",
    "emit_command_result_diagnostic",
    "emit_command_result_message",
    "extra_arguments_result_message",
    "unknown_action_result_message",
]
