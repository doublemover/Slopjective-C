from __future__ import annotations

import sys


DEFAULT_SNIPPET_CHARS = 4000


def bounded_text(value: str, limit: int = DEFAULT_SNIPPET_CHARS) -> str:
    if limit <= 0 or len(value) <= limit:
        return value
    omitted = len(value) - limit
    return f"{value[:limit]}\n... truncated {omitted} character(s) ..."


def normalize_output(value: str | bytes | None) -> str:
    if value is None:
        return ""
    if isinstance(value, bytes):
        return value.decode("utf-8", errors="replace")
    return value


def echo_output(stdout: str, stderr: str) -> None:
    if stdout:
        sys.stdout.write(stdout)
    if stderr:
        sys.stderr.write(stderr)


__all__ = (
    "bounded_text",
    "echo_output",
    "normalize_output",
)
