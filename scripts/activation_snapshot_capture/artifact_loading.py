"""Normalize GitHub issue and milestone artifacts for snapshot output."""

from __future__ import annotations

from typing import Any

from activation_snapshot_capture.models import SnapshotError


def parse_required_number(raw: Any, *, context: str) -> int:
    if isinstance(raw, bool) or not isinstance(raw, int):
        raise SnapshotError(f"{context} must be an integer")
    return raw


def parse_optional_non_negative_int(raw: Any, *, context: str) -> int | None:
    if raw is None:
        return None
    if isinstance(raw, bool) or not isinstance(raw, int):
        raise SnapshotError(f"{context} must be an integer or null")
    if raw < 0:
        raise SnapshotError(f"{context} must be >= 0")
    return raw


def parse_optional_str(raw: Any, *, context: str) -> str | None:
    if raw is None:
        return None
    if not isinstance(raw, str):
        raise SnapshotError(f"{context} must be a string or null")
    value = raw.strip()
    if not value:
        return None
    return value


def parse_labels(raw: Any, *, context: str) -> list[str]:
    if raw is None:
        return []
    if not isinstance(raw, list):
        raise SnapshotError(f"{context} must be an array")

    names: set[str] = set()
    for idx, entry in enumerate(raw):
        label_context = f"{context}[{idx}]"
        if isinstance(entry, str):
            name = entry.strip()
            if name:
                names.add(name)
            continue
        if isinstance(entry, dict):
            candidate = parse_optional_str(
                entry.get("name"),
                context=f"{label_context}.name",
            )
            if candidate is not None:
                names.add(candidate)
            continue
        raise SnapshotError(f"{label_context} must be a string or object")

    return sorted(names, key=lambda item: (item.casefold(), item))


def parse_milestone_ref(raw: Any, *, context: str) -> dict[str, Any] | None:
    if raw is None:
        return None
    if not isinstance(raw, dict):
        raise SnapshotError(f"{context} must be an object or null")

    number_raw = raw.get("number")
    title_raw = raw.get("title")
    if number_raw is None and title_raw is None:
        return None

    number = (
        parse_required_number(number_raw, context=f"{context}.number")
        if number_raw is not None
        else None
    )
    title = parse_optional_str(title_raw, context=f"{context}.title")
    return {
        "number": number,
        "title": title,
    }


def normalize_issue_item(item: Any, *, index: int) -> dict[str, Any]:
    context = f"open issue item {index}"
    if not isinstance(item, dict):
        raise SnapshotError(f"{context} must be an object")

    return {
        "number": parse_required_number(item.get("number"), context=f"{context}.number"),
        "title": parse_optional_str(item.get("title"), context=f"{context}.title"),
        "state": parse_optional_str(item.get("state"), context=f"{context}.state"),
        "url": parse_optional_str(item.get("url"), context=f"{context}.url"),
        "html_url": parse_optional_str(item.get("html_url"), context=f"{context}.html_url"),
        "closed_at": parse_optional_str(item.get("closed_at"), context=f"{context}.closed_at"),
        "labels": parse_labels(item.get("labels"), context=f"{context}.labels"),
        "milestone": parse_milestone_ref(item.get("milestone"), context=f"{context}.milestone"),
    }


def normalize_milestone_item(item: Any, *, index: int) -> dict[str, Any]:
    context = f"open milestone item {index}"
    if not isinstance(item, dict):
        raise SnapshotError(f"{context} must be an object")

    return {
        "number": parse_required_number(item.get("number"), context=f"{context}.number"),
        "title": parse_optional_str(item.get("title"), context=f"{context}.title"),
        "state": parse_optional_str(item.get("state"), context=f"{context}.state"),
        "description": parse_optional_str(
            item.get("description"),
            context=f"{context}.description",
        ),
        "open_issues": parse_optional_non_negative_int(
            item.get("open_issues"),
            context=f"{context}.open_issues",
        ),
        "closed_issues": parse_optional_non_negative_int(
            item.get("closed_issues"),
            context=f"{context}.closed_issues",
        ),
        "url": parse_optional_str(item.get("url"), context=f"{context}.url"),
        "html_url": parse_optional_str(item.get("html_url"), context=f"{context}.html_url"),
        "created_at": parse_optional_str(item.get("created_at"), context=f"{context}.created_at"),
        "updated_at": parse_optional_str(item.get("updated_at"), context=f"{context}.updated_at"),
        "due_on": parse_optional_str(item.get("due_on"), context=f"{context}.due_on"),
        "closed_at": parse_optional_str(item.get("closed_at"), context=f"{context}.closed_at"),
    }


__all__ = [
    "normalize_issue_item",
    "normalize_milestone_item",
    "parse_labels",
    "parse_milestone_ref",
    "parse_optional_non_negative_int",
    "parse_optional_str",
    "parse_required_number",
]
