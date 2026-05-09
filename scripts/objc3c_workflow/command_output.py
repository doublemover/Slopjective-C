"""Command output parsing helpers for objc3c workflow actions."""

from __future__ import annotations


def extract_output_line(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


__all__ = ["extract_output_line"]
