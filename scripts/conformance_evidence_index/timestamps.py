from __future__ import annotations

import os
from datetime import datetime, timezone


class StrictGeneratedAtError(ValueError):
    """Raised when strict source generated_at validation fails."""


def parse_rfc3339_utc(value: str) -> str:
    raw = value.strip()
    if not raw:
        raise ValueError("timestamp cannot be empty")

    normalized = raw
    if raw.endswith("Z"):
        normalized = raw[:-1] + "+00:00"

    try:
        parsed = datetime.fromisoformat(normalized)
    except ValueError as exc:
        raise ValueError(
            f"invalid timestamp {value!r}; expected RFC3339 date-time"
        ) from exc

    if parsed.tzinfo is None:
        raise ValueError(
            f"invalid timestamp {value!r}; timezone offset or Z suffix is required"
        )

    canonical = parsed.astimezone(timezone.utc).replace(microsecond=0)
    return canonical.isoformat().replace("+00:00", "Z")


def source_date_epoch_to_utc(value: str) -> str:
    try:
        epoch = int(value)
    except ValueError as exc:
        raise ValueError(
            f"SOURCE_DATE_EPOCH must be an integer; got {value!r}"
        ) from exc

    if epoch < 0:
        raise ValueError("SOURCE_DATE_EPOCH must be >= 0")

    parsed = datetime.fromtimestamp(epoch, tz=timezone.utc).replace(microsecond=0)
    return parsed.isoformat().replace("+00:00", "Z")


def resolve_index_generated_at(explicit_generated_at: str | None) -> str | None:
    if explicit_generated_at:
        return parse_rfc3339_utc(explicit_generated_at)

    source_date_epoch = os.getenv("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return source_date_epoch_to_utc(source_date_epoch)

    return None
