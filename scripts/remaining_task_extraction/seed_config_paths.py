"""Path and environment resolution for seed configuration."""

from __future__ import annotations

import os
from datetime import date, datetime, timezone
from pathlib import Path

from remaining_task_extraction.seed_config_models import ConfigError
from remaining_task_extraction.seed_config_validation import parse_iso_date


def resolve_root_path(raw_path: str, *, root: Path) -> Path:
    path = Path(raw_path)
    if path.is_absolute():
        return path
    return root / path


def resolve_config_path(config_path: Path, *, root: Path) -> Path:
    if config_path.is_absolute():
        return config_path
    return root / config_path


def resolve_generated_on(raw_generated_on: str | None) -> date:
    if raw_generated_on is not None:
        return parse_iso_date(raw_generated_on, context="--generated-on")

    raw_epoch = os.environ.get("SOURCE_DATE_EPOCH")
    if raw_epoch is None or not raw_epoch.strip():
        return date.today()

    try:
        epoch_seconds = int(raw_epoch)
    except ValueError as exc:
        raise ConfigError(f"SOURCE_DATE_EPOCH must be an integer Unix timestamp (found '{raw_epoch}')") from exc

    if epoch_seconds < 0:
        raise ConfigError("SOURCE_DATE_EPOCH must be a non-negative Unix timestamp")

    try:
        return datetime.fromtimestamp(epoch_seconds, tz=timezone.utc).date()
    except (OverflowError, OSError, ValueError) as exc:
        raise ConfigError(f"SOURCE_DATE_EPOCH is out of supported range (found '{raw_epoch}')") from exc


__all__ = [
    "resolve_config_path",
    "resolve_generated_on",
    "resolve_root_path",
]
