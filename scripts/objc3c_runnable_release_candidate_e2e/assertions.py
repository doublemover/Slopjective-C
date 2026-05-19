"""Assertions for runnable release-candidate end-to-end validation."""

from __future__ import annotations


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


__all__ = ["expect"]
