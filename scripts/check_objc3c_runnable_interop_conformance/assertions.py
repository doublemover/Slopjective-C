"""Fail-closed assertion helpers for runnable interop conformance."""

from __future__ import annotations


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


__all__ = ["expect"]
