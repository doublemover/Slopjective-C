"""Capture raw activation snapshot artifacts from GitHub."""

from __future__ import annotations

from typing import Any

from activation_snapshot_capture.artifact_loading import (
    normalize_issue_item,
    normalize_milestone_item,
)
from activation_snapshot_capture.constants import MILESTONES_ENDPOINT
from activation_snapshot_capture.models import GhClientLike, SnapshotError


def collect_open_milestones(client: GhClientLike) -> list[dict[str, Any]]:
    payload = client.api_json(MILESTONES_ENDPOINT, paginate=True)
    if not isinstance(payload, list):
        raise SnapshotError(
            "open milestone payload from GitHub CLI must be a paginated list"
        )

    items: list[dict[str, Any]] = []
    for page_index, page in enumerate(payload):
        page_context = f"open milestones page {page_index}"
        if not isinstance(page, list):
            raise SnapshotError(f"{page_context} must be an array")
        for item_index, raw_item in enumerate(page):
            item = normalize_milestone_item(
                raw_item,
                index=len(items) + item_index,
            )
            items.append(item)
    return items


def collect_open_issues(client: GhClientLike) -> list[dict[str, Any]]:
    raw_items = client.list_issues(state="open")
    return [
        normalize_issue_item(raw_item, index=index)
        for index, raw_item in enumerate(raw_items)
    ]


__all__ = ["collect_open_issues", "collect_open_milestones"]
