from __future__ import annotations

from typing import Any


def check_key_order(
    payload: dict[str, Any],
    *,
    expected: list[str],
    label: str,
) -> list[str]:
    observed = list(payload.keys())
    if observed == expected:
        return []
    return [f"{label} key order drift: expected={expected!r} observed={observed!r}."]
