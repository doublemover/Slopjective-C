"""Strict CLI value validation for open-blocker audits."""

from __future__ import annotations

from datetime import datetime

from .audit_scope_constants import ISO_UTC_SECOND_RE


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def validate_generated_at_utc(raw_value: str) -> str:
    value = raw_value.strip()
    if value != raw_value:
        raise ValueError(
            "invalid --generated-at-utc: value must not include leading or trailing whitespace"
        )
    if not ISO_UTC_SECOND_RE.fullmatch(value):
        raise ValueError(
            "invalid --generated-at-utc: expected strict UTC timestamp like YYYY-MM-DDTHH:MM:SSZ"
        )
    try:
        datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise ValueError(
            "invalid --generated-at-utc: timestamp is not a valid UTC date-time"
        ) from exc
    return value


def validate_snapshot_source(raw_value: str) -> str:
    value = raw_value.strip()
    if not value:
        raise ValueError("invalid --source: value must be a non-empty canonical string")
    if value != raw_value:
        raise ValueError(
            "invalid --source: value must not include leading or trailing whitespace"
        )
    if normalize_space(value) != value:
        raise ValueError(
            "invalid --source: value must be canonical (no repeated internal whitespace)"
        )
    return value


__all__ = (
    "normalize_space",
    "validate_generated_at_utc",
    "validate_snapshot_source",
)
