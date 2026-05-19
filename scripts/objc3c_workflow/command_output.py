"""Command output parsing helpers for objc3c workflow actions."""

from __future__ import annotations

from .command_output_lines import extract_prefixed_value


def extract_output_line(stdout: str, prefix: str) -> str:
    return extract_prefixed_value(stdout, prefix)


__all__ = ["extract_output_line"]
