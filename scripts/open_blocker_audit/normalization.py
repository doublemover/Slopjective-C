"""Open-blocker audit metadata and row normalization."""

from __future__ import annotations

import json

from scripts.open_blocker_extraction.audit_payload import (
    validate_extract_snapshot_payload,
)
from scripts.open_blocker_extraction.audit_runner import build_runner_snapshot_payload
from scripts.open_blocker_extraction.audit_scope import (
    validate_generated_at_utc,
    validate_snapshot_source,
)

from .models import SnapshotMetadata


def normalize_snapshot_metadata(
    *,
    raw_generated_at_utc: str | None,
    raw_source: str | None,
) -> tuple[SnapshotMetadata, list[str]]:
    errors: list[str] = []
    generated_at_utc: str | None = None
    source: str | None = None

    if raw_generated_at_utc is None:
        errors.append("--generated-at-utc is required.")
    else:
        try:
            generated_at_utc = validate_generated_at_utc(raw_generated_at_utc)
        except ValueError as exc:
            errors.append(str(exc))

    if raw_source is None:
        errors.append("--source is required.")
    else:
        try:
            source = validate_snapshot_source(raw_source)
        except ValueError as exc:
            errors.append(str(exc))

    return SnapshotMetadata(generated_at_utc=generated_at_utc, source=source), errors


def normalize_extract_snapshot_stdout(
    stdout: str,
    *,
    generated_at_utc: str,
    source: str,
) -> tuple[dict[str, object] | None, list[str]]:
    errors: list[str] = []
    try:
        raw_payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        return (
            None,
            [
                "extract_open_blockers(snapshot-json) emitted invalid JSON: "
                f"{exc.msg} at {exc.lineno}:{exc.colno}."
            ],
        )

    if not isinstance(raw_payload, dict):
        return (
            None,
            ["extract_open_blockers(snapshot-json) output root must be an object."],
        )

    try:
        normalized_extract_payload = validate_extract_snapshot_payload(
            raw_payload,
            expected_generated_at_utc=generated_at_utc,
            expected_source=source,
        )
        normalized_snapshot_payload = build_runner_snapshot_payload(
            normalized_extract_payload
        )
    except ValueError as exc:
        errors.append(str(exc))
        normalized_snapshot_payload = None

    return normalized_snapshot_payload, errors


def extract_blocker_count(snapshot_payload: dict[str, object] | None) -> int | None:
    if snapshot_payload is None:
        return None

    raw_count = snapshot_payload.get("open_blocker_count")
    if isinstance(raw_count, int) and not isinstance(raw_count, bool):
        return raw_count

    return None


__all__ = [
    "build_runner_snapshot_payload",
    "extract_blocker_count",
    "normalize_extract_snapshot_stdout",
    "normalize_snapshot_metadata",
    "validate_extract_snapshot_payload",
    "validate_generated_at_utc",
    "validate_snapshot_source",
]
