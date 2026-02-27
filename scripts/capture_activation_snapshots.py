#!/usr/bin/env python3
"""Capture deterministic activation snapshots for open issues and milestones."""

from __future__ import annotations

import argparse
import json
import os
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS_DIR = Path(__file__).resolve().parent
ISSUES_ENDPOINT = "repos/{owner}/{repo}/issues?state=open&per_page=100"
MILESTONES_ENDPOINT = "repos/{owner}/{repo}/milestones?state=open&per_page=100"
ISSUES_SOURCE = (
    "gh api repos/{owner}/{repo}/issues?state=open&per_page=100 "
    "--paginate --slurp (pull_request filtered)"
)
MILESTONES_SOURCE = (
    "gh api repos/{owner}/{repo}/milestones?state=open&per_page=100 "
    "--paginate --slurp"
)

if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

from lib.gh_client import GhClient, GhClientError


class SnapshotError(RuntimeError):
    """Raised when snapshot inputs cannot be normalized deterministically."""


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_repo_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


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

    # Canonicalize to guarantee stable formatting.
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


def collect_open_milestones(client: GhClient) -> list[dict[str, Any]]:
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


def collect_open_issues(client: GhClient) -> list[dict[str, Any]]:
    raw_items = client.list_issues(state="open")
    return [
        normalize_issue_item(raw_item, index=index)
        for index, raw_item in enumerate(raw_items)
    ]


def render_json(payload: dict[str, Any]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="capture_activation_snapshots.py",
        description=(
            "Capture deterministic JSON snapshots of open issues and open milestones "
            "for activation preflight workflows."
        ),
    )
    parser.add_argument(
        "--issues-output",
        type=Path,
        required=True,
        help="Output path for open-issues snapshot JSON.",
    )
    parser.add_argument(
        "--milestones-output",
        type=Path,
        required=True,
        help="Output path for open-milestones snapshot JSON.",
    )
    parser.add_argument(
        "--generated-at-utc",
        help=(
            "Optional explicit generated-at timestamp (RFC3339 UTC, "
            "YYYY-MM-DDTHH:MM:SSZ). If omitted, SOURCE_DATE_EPOCH is used when set; "
            "otherwise current UTC time is used."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    issues_output = resolve_repo_path(args.issues_output)
    milestones_output = resolve_repo_path(args.milestones_output)

    try:
        generated_at_utc = resolve_generated_at_utc(args.generated_at_utc)
        client = GhClient(root=ROOT)
        issue_items = collect_open_issues(client)
        milestone_items = collect_open_milestones(client)

        issues_snapshot = build_snapshot(
            generated_at_utc=generated_at_utc,
            source=ISSUES_SOURCE,
            items=issue_items,
        )
        milestones_snapshot = build_snapshot(
            generated_at_utc=generated_at_utc,
            source=MILESTONES_SOURCE,
            items=milestone_items,
        )

        write_text(issues_output, render_json(issues_snapshot))
        write_text(milestones_output, render_json(milestones_snapshot))
    except (GhClientError, SnapshotError, OSError, ValueError) as exc:
        print(f"capture-activation-snapshots: {exc}", file=sys.stderr)
        return 1

    print(
        "capture-activation-snapshots: OK "
        f"(issues={issues_snapshot['count']}, milestones={milestones_snapshot['count']}, "
        f"issues_output={display_path(issues_output)}, "
        f"milestones_output={display_path(milestones_output)})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
