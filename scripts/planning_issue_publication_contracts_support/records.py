from __future__ import annotations

from typing import Any


def milestone_by_title(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {milestone["title"]: milestone for milestone in payload["milestones"]}


def issue_number(mapping: dict[str, Any], draft_id: str) -> int | None:
    record = mapping.get(draft_id)
    if isinstance(record, dict) and isinstance(record.get("number"), int):
        return record["number"]
    return None
