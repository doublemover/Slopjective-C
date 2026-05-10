from __future__ import annotations

import argparse
import os
from datetime import date, datetime, timezone


def parse_generated_on(raw: str) -> date:
    try:
        parsed = date.fromisoformat(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "--generated-on must be in YYYY-MM-DD format"
        ) from exc
    return parsed


def source_date_epoch_to_date(value: str) -> date:
    try:
        epoch = int(value)
    except ValueError as exc:
        raise ValueError(f"SOURCE_DATE_EPOCH must be an integer; got {value!r}") from exc

    if epoch < 0:
        raise ValueError("SOURCE_DATE_EPOCH must be >= 0")

    try:
        return datetime.fromtimestamp(epoch, tz=timezone.utc).date()
    except (OverflowError, OSError, ValueError) as exc:
        raise ValueError(
            f"SOURCE_DATE_EPOCH is out of range for UTC conversion: {value!r}"
        ) from exc


def resolve_generated_on(*, generated_on: date | None, snapshot_date: date | None) -> date:
    if generated_on is not None:
        return generated_on

    if snapshot_date is not None:
        return snapshot_date

    source_date_epoch = os.getenv("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return source_date_epoch_to_date(source_date_epoch)

    raise ValueError(
        "must provide --generated-on (or legacy --snapshot-date) when SOURCE_DATE_EPOCH is not set"
    )
