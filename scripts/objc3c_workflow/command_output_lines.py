"""Owned line extraction contracts for subprocess output."""

from __future__ import annotations

COMMAND_OUTPUT_LINE_OWNER = "objc3c-workflow-command-output-lines"
COMMAND_OUTPUT_MISSING_LINE = ""


def normalized_output_line(raw_line: str) -> str:
    return raw_line.strip()


def extract_prefixed_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = normalized_output_line(raw_line)
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return COMMAND_OUTPUT_MISSING_LINE


__all__ = [
    "COMMAND_OUTPUT_LINE_OWNER",
    "COMMAND_OUTPUT_MISSING_LINE",
    "extract_prefixed_value",
    "normalized_output_line",
]
