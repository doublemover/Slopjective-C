from __future__ import annotations

from typing import Any


def format_finding(finding: dict[str, Any]) -> str:
    return f"{finding['path']}:{finding['line']}: {finding['pattern_id']}: {finding['excerpt']}"
