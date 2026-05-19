"""Expectation matching primitives for runtime acceptance."""

from __future__ import annotations

from typing import Any


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def expect_equal(observed: Any, expected: Any, message: str) -> None:
    if observed != expected:
        raise RuntimeError(f"{message}: expected {expected!r}, observed {observed!r}")


def expect_mapping_field(
    payload: dict[str, Any],
    field: str,
    expected: Any,
    message: str,
) -> None:
    expect_equal(payload.get(field), expected, message)


def expect_required_mapping(
    payload: dict[str, Any],
    field: str,
    message: str,
) -> dict[str, Any]:
    value = payload.get(field)
    if not isinstance(value, dict):
        raise RuntimeError(message)
    return value


__all__ = [
    "expect",
    "expect_equal",
    "expect_mapping_field",
    "expect_required_mapping",
]
