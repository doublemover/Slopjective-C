"""Low-level request diagnostic output."""

from __future__ import annotations

from .request_error_policy import DEFAULT_REQUEST_ERROR_EXIT_CODE, request_error_exit_code, write_request_error


def emit_request_error(message: str, exit_code: int | None = DEFAULT_REQUEST_ERROR_EXIT_CODE) -> int:
    write_request_error(message)
    return request_error_exit_code(exit_code)


__all__ = ["emit_request_error"]
