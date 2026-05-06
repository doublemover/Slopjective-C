"""Common report envelope helpers."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any


def report_envelope(
    *,
    contract_id: str,
    status: str,
    generated_by: str,
    payload: Mapping[str, Any],
) -> dict[str, Any]:
    if status not in {"PASS", "FAIL"}:
        raise ValueError("report status must be PASS or FAIL")
    return {
        "contract_id": contract_id,
        "status": status,
        "generated_by": generated_by,
        "payload": dict(payload),
    }
