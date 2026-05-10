"""Open-blocker audit extractor snapshot payload validation."""

from __future__ import annotations

import re

WINDOWS_ABSOLUTE_RE = re.compile(r"^[A-Za-z]:/")


def parse_non_negative_int(value: object, *, field_path: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value < 0:
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must be a non-negative integer."
        )
    return value


def parse_positive_int(value: object, *, field_path: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value <= 0:
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must be a positive integer."
        )
    return value


def validate_source_path(source_path: object, *, field_path: str) -> str:
    if not isinstance(source_path, str) or not source_path:
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must be a non-empty string."
        )
    if source_path.strip() != source_path:
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must not include leading/trailing whitespace."
        )
    if "\\" in source_path:
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must use '/' path separators."
        )
    if source_path.startswith("/") or WINDOWS_ABSOLUTE_RE.match(source_path):
        raise ValueError(
            "extract_open_blockers(snapshot-json) field "
            f"{field_path!r} must be a relative path."
        )
    return source_path


def validate_extract_snapshot_payload(
    payload: dict[str, object],
    *,
    expected_generated_at_utc: str,
    expected_source: str,
) -> dict[str, object]:
    expected_root_key_order = [
        "generated_at_utc",
        "source",
        "open_blocker_count",
        "open_blockers",
    ]
    observed_root_key_order = list(payload.keys())
    if observed_root_key_order != expected_root_key_order:
        raise ValueError(
            "extract_open_blockers(snapshot-json) root key order drift: "
            f"expected={expected_root_key_order!r} observed={observed_root_key_order!r}."
        )

    generated_at_utc = payload.get("generated_at_utc")
    if generated_at_utc != expected_generated_at_utc:
        raise ValueError(
            "extract_open_blockers(snapshot-json) metadata drift for generated_at_utc: "
            f"observed={generated_at_utc!r} expected={expected_generated_at_utc!r}."
        )

    source = payload.get("source")
    if source != expected_source:
        raise ValueError(
            "extract_open_blockers(snapshot-json) metadata drift for source: "
            f"observed={source!r} expected={expected_source!r}."
        )

    raw_rows = payload.get("open_blockers")
    if not isinstance(raw_rows, list):
        raise ValueError(
            "extract_open_blockers(snapshot-json) root field 'open_blockers' must be a list."
        )

    expected_row_key_order = ["blocker_id", "source_path", "line_number", "line"]
    canonical_rows: list[dict[str, object]] = []
    seen_rows: set[tuple[str, str, int]] = set()

    for index, raw_row in enumerate(raw_rows):
        field_path = f"open_blockers[{index}]"
        if not isinstance(raw_row, dict):
            raise ValueError(
                "extract_open_blockers(snapshot-json) field "
                f"{field_path!r} must be an object."
            )
        observed_row_key_order = list(raw_row.keys())
        if observed_row_key_order != expected_row_key_order:
            raise ValueError(
                "extract_open_blockers(snapshot-json) row key order drift for "
                f"{field_path!r}: expected={expected_row_key_order!r} "
                f"observed={observed_row_key_order!r}."
            )

        blocker_id = raw_row.get("blocker_id")
        if not isinstance(blocker_id, str) or not blocker_id:
            raise ValueError(
                "extract_open_blockers(snapshot-json) field "
                f"{field_path}.blocker_id must be a non-empty string."
            )
        if blocker_id.strip() != blocker_id:
            raise ValueError(
                "extract_open_blockers(snapshot-json) field "
                f"{field_path}.blocker_id must not include leading/trailing whitespace."
            )

        source_path = validate_source_path(
            raw_row.get("source_path"),
            field_path=f"{field_path}.source_path",
        )
        line_number = parse_positive_int(
            raw_row.get("line_number"),
            field_path=f"{field_path}.line_number",
        )
        line_alias = parse_positive_int(raw_row.get("line"), field_path=f"{field_path}.line")
        if line_alias != line_number:
            raise ValueError(
                "extract_open_blockers(snapshot-json) line alias mismatch for "
                f"{field_path}: line_number={line_number} line={line_alias}."
            )

        row_identity = (blocker_id, source_path, line_number)
        if row_identity in seen_rows:
            raise ValueError(
                "extract_open_blockers(snapshot-json) duplicate canonical row "
                f"{row_identity!r}."
            )
        seen_rows.add(row_identity)

        canonical_rows.append(
            {
                "blocker_id": blocker_id,
                "source_path": source_path,
                "line_number": line_number,
                "line": line_number,
            }
        )

    declared_count = parse_non_negative_int(
        payload.get("open_blocker_count"),
        field_path="open_blocker_count",
    )
    if declared_count != len(canonical_rows):
        raise ValueError(
            "extract_open_blockers(snapshot-json) count mismatch: "
            f"open_blocker_count={declared_count} discovered={len(canonical_rows)}."
        )

    expected_order = sorted(
        canonical_rows,
        key=lambda row: (
            str(row["source_path"]),
            int(row["line_number"]),
            str(row["blocker_id"]),
        ),
    )
    if canonical_rows != expected_order:
        raise ValueError(
            "extract_open_blockers(snapshot-json) canonical rows must be sorted by "
            "'source_path', then line number, then 'blocker_id'."
        )

    return {
        "generated_at_utc": expected_generated_at_utc,
        "source": expected_source,
        "open_blocker_count": len(canonical_rows),
        "open_blockers": canonical_rows,
    }


__all__ = ["validate_extract_snapshot_payload"]
