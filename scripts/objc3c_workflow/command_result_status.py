"""Canonical workflow command result status strings."""

from __future__ import annotations

STATUS_ACCEPTED = "accepted"
STATUS_COMPLETED = "completed"
STATUS_FAILED = "failed"
STATUS_EXTRA_ARGUMENTS_REJECTED = "extra-arguments-rejected"
STATUS_UNKNOWN_ACTION = "unknown-action"

REJECTED_STATUSES = frozenset(
    {
        STATUS_EXTRA_ARGUMENTS_REJECTED,
        STATUS_UNKNOWN_ACTION,
    }
)


def completion_status(exit_code: int) -> str:
    return STATUS_COMPLETED if exit_code == 0 else STATUS_FAILED


__all__ = [
    "REJECTED_STATUSES",
    "STATUS_ACCEPTED",
    "STATUS_COMPLETED",
    "STATUS_EXTRA_ARGUMENTS_REJECTED",
    "STATUS_FAILED",
    "STATUS_UNKNOWN_ACTION",
    "completion_status",
]
