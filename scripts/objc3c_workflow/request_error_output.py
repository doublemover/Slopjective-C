"""Low-level request diagnostic output."""

from __future__ import annotations

import sys


def emit_request_error(message: str, exit_code: int = 2) -> int:
    print(message, file=sys.stderr)
    return exit_code


__all__ = ["emit_request_error"]
