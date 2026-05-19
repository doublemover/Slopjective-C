"""Comparative result status helpers."""

from __future__ import annotations


def status_from_failures(failures: list[str]) -> str:
    return "PASS" if not failures else "FAIL"
