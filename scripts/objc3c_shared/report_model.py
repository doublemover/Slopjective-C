"""Common report envelope helpers."""

from __future__ import annotations

from collections.abc import Mapping
from typing import Any

REPORT_STATUSES = frozenset({"PASS", "FAIL"})


def _require_non_empty_string(value: Any, field: str) -> str:
    if not isinstance(value, str) or not value:
        raise ValueError(f"report {field} must be a non-empty string")
    return value


def _require_status(value: Any) -> str:
    status = _require_non_empty_string(value, "status")
    if status not in REPORT_STATUSES:
        raise ValueError("report status must be PASS or FAIL")
    return status


def report_envelope(
    *,
    contract_id: str,
    status: str,
    generated_by: str,
    payload: Mapping[str, Any],
    schema_id: str | None = None,
) -> dict[str, Any]:
    if not isinstance(payload, Mapping):
        raise ValueError("report payload must be an object")
    envelope: dict[str, Any] = {
        "contract_id": _require_non_empty_string(contract_id, "contract_id"),
        "status": _require_status(status),
        "generated_by": _require_non_empty_string(generated_by, "generated_by"),
    }
    if schema_id is not None:
        envelope["schema_id"] = _require_non_empty_string(schema_id, "schema_id")
    envelope["payload"] = dict(payload)
    validate_report_envelope(envelope, require_schema_id=schema_id is not None)
    return envelope


def validate_report_envelope(
    report: Mapping[str, Any],
    *,
    require_schema_id: bool = False,
) -> None:
    if not isinstance(report, Mapping):
        raise ValueError("report envelope must be an object")
    _require_non_empty_string(report.get("contract_id"), "contract_id")
    _require_status(report.get("status"))
    _require_non_empty_string(report.get("generated_by"), "generated_by")
    if require_schema_id or "schema_id" in report:
        _require_non_empty_string(report.get("schema_id"), "schema_id")
    if not isinstance(report.get("payload"), Mapping):
        raise ValueError("report payload must be an object")
