"""Owned exit-code policy for workflow command results."""

from __future__ import annotations

COMMAND_RESULT_POLICY_OWNER = "objc3c-workflow-command-result-policy"
COMMAND_RESULT_ACCEPTED_EXIT_CODE = 0
COMMAND_RESULT_REJECTED_EXIT_CODE = 2


__all__ = [
    "COMMAND_RESULT_ACCEPTED_EXIT_CODE",
    "COMMAND_RESULT_POLICY_OWNER",
    "COMMAND_RESULT_REJECTED_EXIT_CODE",
]
