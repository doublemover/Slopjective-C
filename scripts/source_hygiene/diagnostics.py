from __future__ import annotations

from typing import Any

from .violations import format_violation


def format_finding(finding: dict[str, Any]) -> str:
    return format_violation(finding)
