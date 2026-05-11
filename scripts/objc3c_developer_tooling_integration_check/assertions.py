"""Assertion helpers for developer-tooling integration reports."""

from __future__ import annotations


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)
