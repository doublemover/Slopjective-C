"""Owned stderr and exit-code policy for workflow request errors."""

from __future__ import annotations

import sys
from typing import TextIO

REQUEST_ERROR_OUTPUT_OWNER = "objc3c-workflow-request-error-output"
REQUEST_ERROR_STREAM_OWNER = "objc3c-workflow-request-error-stderr"
REQUEST_ERROR_EXIT_CODE_OWNER = "objc3c-workflow-request-error-exit-code"
DEFAULT_REQUEST_ERROR_EXIT_CODE = 2


def request_error_stream() -> TextIO:
    return sys.stderr


def request_error_exit_code(exit_code: int | None = None) -> int:
    return DEFAULT_REQUEST_ERROR_EXIT_CODE if exit_code is None else exit_code


def write_request_error(message: str) -> None:
    print(message, file=request_error_stream())


__all__ = [
    "DEFAULT_REQUEST_ERROR_EXIT_CODE",
    "REQUEST_ERROR_EXIT_CODE_OWNER",
    "REQUEST_ERROR_OUTPUT_OWNER",
    "REQUEST_ERROR_STREAM_OWNER",
    "request_error_exit_code",
    "request_error_stream",
    "write_request_error",
]
