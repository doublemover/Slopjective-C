"""Usage text for the objc3c workflow CLI."""

from __future__ import annotations

from .environment import WORKFLOW_COMMAND_TEXT


def usage_text() -> str:
    return (
        f"usage: {WORKFLOW_COMMAND_TEXT} <action> [args...]\n"
        f"       {WORKFLOW_COMMAND_TEXT} --list-json\n"
        f"       {WORKFLOW_COMMAND_TEXT} --describe <action>\n"
        f"       {WORKFLOW_COMMAND_TEXT} --describe-script <package-script>"
    )


__all__ = ["usage_text"]
