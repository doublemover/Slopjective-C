"""Timestamp and JSON snapshot shaping helpers."""

from __future__ import annotations

import os
from datetime import datetime, timezone
from typing import Any


def parse_generated_at_utc(raw: str) -> str:
    value = raw.strip()
    if not value:
        raise ValueError("--generated-at-utc must be non-empty")
    try:
        parsed = datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise ValueError(
            "--generated-at-utc must be RFC3339 UTC with second precision "
            "(YYYY-MM-DDTHH:MM:SSZ)"
        ) from exc

    return parsed.replace(tzinfo=timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def source_date_epoch_to_generated_at_utc(raw: str) -> str:
    try:
        epoch = int(raw)
    except ValueError as exc:
        raise ValueError(f"SOURCE_DATE_EPOCH must be an integer; got {raw!r}") from exc

    if epoch < 0:
        raise ValueError("SOURCE_DATE_EPOCH must be >= 0")

    try:
        generated_at = datetime.fromtimestamp(epoch, tz=timezone.utc)
    except (OverflowError, OSError, ValueError) as exc:
        raise ValueError(
            f"SOURCE_DATE_EPOCH is out of range for UTC conversion: {raw!r}"
        ) from exc

    return generated_at.replace(microsecond=0).strftime("%Y-%m-%dT%H:%M:%SZ")


def resolve_generated_at_utc(explicit: str | None) -> str:
    if explicit is not None:
        return parse_generated_at_utc(explicit)

    source_date_epoch = os.getenv("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return source_date_epoch_to_generated_at_utc(source_date_epoch)

    return datetime.now(timezone.utc).replace(microsecond=0).strftime(
        "%Y-%m-%dT%H:%M:%SZ"
    )


def sort_items_by_number(items: list[dict[str, Any]]) -> list[dict[str, Any]]:
    return sorted(items, key=lambda item: item["number"])


def build_snapshot(
    *,
    generated_at_utc: str,
    source: str,
    items: list[dict[str, Any]],
) -> dict[str, Any]:
    sorted_items = sort_items_by_number(items)
    return {
        "generated_at_utc": generated_at_utc,
        "source": source,
        "count": len(sorted_items),
        "items": sorted_items,
    }


__all__ = [
    "build_snapshot",
    "parse_generated_at_utc",
    "resolve_generated_at_utc",
    "sort_items_by_number",
    "source_date_epoch_to_generated_at_utc",
]
