"""Shared assertion helpers for platform-hardening integration checks."""

from __future__ import annotations


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)
