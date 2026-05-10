"""Assertion helpers for runnable interop end-to-end validation."""

from __future__ import annotations


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def expect_probe_fields(
    payload: dict[str, object],
    expected_values: dict[str, object],
    *,
    context: str,
) -> None:
    for field_name, expected_value in expected_values.items():
        expect(
            payload.get(field_name) == expected_value,
            f"expected {context} to preserve {field_name}",
        )


__all__ = ["expect", "expect_probe_fields"]
