"""Open blocker serialization, snapshot metadata validation, and markdown rendering."""

from __future__ import annotations

import json
from datetime import datetime
from typing import Any, Sequence

from open_blocker_extraction.markdown_scan import normalize_space
from open_blocker_extraction.model import ISO_UTC_SECOND_RE, OpenBlocker


def blocker_to_dict(row: OpenBlocker) -> dict[str, object]:
    return {
        "blocker_id": row.blocker_id,
        "source_path": row.source_path,
        "line": row.line,
        "owner": row.owner,
        "due_date_utc": row.due_date_utc,
        "summary": row.summary,
        "status": row.status,
    }


def render_json(rows: Sequence[OpenBlocker]) -> str:
    payload = [blocker_to_dict(row) for row in rows]
    return json.dumps(payload, indent=2) + "\n"


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


def validate_snapshot_args(args: Any) -> tuple[str, str] | None:
    if args.format != "snapshot-json":
        if args.generated_at_utc is not None:
            raise ValueError(
                "--generated-at-utc is only supported when --format snapshot-json"
            )
        if args.source is not None:
            raise ValueError("--source is only supported when --format snapshot-json")
        return None

    if args.generated_at_utc is None:
        raise ValueError(
            "--generated-at-utc is required when --format snapshot-json"
        )
    if args.source is None:
        raise ValueError("--source is required when --format snapshot-json")

    return (
        validate_generated_at_utc(args.generated_at_utc),
        validate_snapshot_source(args.source),
    )


def snapshot_row_to_dict(row: OpenBlocker) -> dict[str, object]:
    line_number = row.line
    return {
        "blocker_id": row.blocker_id,
        "source_path": row.source_path,
        "line_number": line_number,
        "line": line_number,
    }


def render_snapshot_json(
    rows: Sequence[OpenBlocker],
    *,
    generated_at_utc: str,
    source: str,
) -> str:
    payload = {
        "generated_at_utc": generated_at_utc,
        "source": source,
        "open_blocker_count": len(rows),
        "open_blockers": [snapshot_row_to_dict(row) for row in rows],
    }
    return json.dumps(payload, indent=2) + "\n"


def escape_markdown_cell(value: str) -> str:
    return value.replace("|", r"\|")


def render_markdown(rows: Sequence[OpenBlocker]) -> str:
    lines = ["# Open blockers", ""]
    if not rows:
        lines.append("_No open blockers found._")
        return "\n".join(lines).rstrip() + "\n"

    lines.extend(
        [
            "| blocker_id | source_path | line | owner | due_date_utc | summary | status |",
            "| --- | --- | --- | --- | --- | --- | --- |",
        ]
    )
    for row in rows:
        due_value = f"`{row.due_date_utc}`" if row.due_date_utc is not None else "_none_"
        lines.append(
            "| "
            f"`{row.blocker_id}` | "
            f"`{row.source_path}` | "
            f"{row.line} | "
            f"{escape_markdown_cell(row.owner)} | "
            f"{due_value} | "
            f"{escape_markdown_cell(row.summary)} | "
            f"`{row.status}` |"
        )
    return "\n".join(lines).rstrip() + "\n"

