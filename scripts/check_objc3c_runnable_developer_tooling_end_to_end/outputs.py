"""Output parsing helpers for runnable developer-tooling end-to-end validation."""

from __future__ import annotations


def extract_last_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    value: str | None = None
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            value = line.split(":", 1)[1].strip()
    return value


__all__ = ["extract_last_output_value"]
