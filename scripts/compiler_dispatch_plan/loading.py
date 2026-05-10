from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from compiler_dispatch_plan.model import IssueRow
from compiler_dispatch_plan.normalization import normalize_space
from compiler_dispatch_plan.owner_contract import validate_fixture_owner_contract


def flatten_json_pages(payload: Any) -> list[dict[str, Any]]:
    if isinstance(payload, dict):
        pages = payload.get("pages")
        if pages is None:
            raise ValueError("issues JSON object must include a pages array")
        return flatten_json_pages(pages)

    if isinstance(payload, list):
        flattened: list[dict[str, Any]] = []
        if payload and all(isinstance(item, list) for item in payload):
            for page in payload:
                for row in page:
                    if isinstance(row, dict):
                        flattened.append(row)
            return flattened

        for row in payload:
            if isinstance(row, dict):
                flattened.append(row)
        return flattened
    raise ValueError("issues JSON must be an array or slurped page-array")


def parse_lane_labels(raw_labels: Any) -> tuple[str, ...]:
    if not isinstance(raw_labels, list):
        return ()
    lanes: set[str] = set()
    for raw in raw_labels:
        if not isinstance(raw, dict):
            continue
        name = raw.get("name")
        if not isinstance(name, str):
            continue
        if name.startswith("lane:"):
            lanes.add(name)
    return tuple(sorted(lanes))


def parse_issue_rows(path: Path) -> list[IssueRow]:
    try:
        raw_payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"issues JSON file does not exist: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read issues JSON file {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {display_path(path)}: {exc}") from exc

    validate_fixture_owner_contract(raw_payload)

    rows: list[IssueRow] = []
    for raw in flatten_json_pages(raw_payload):
        if "pull_request" in raw:
            continue

        milestone = raw.get("milestone")
        if not isinstance(milestone, dict):
            continue
        milestone_number = milestone.get("number")
        milestone_title = milestone.get("title")
        number = raw.get("number")
        title = raw.get("title")
        if not isinstance(milestone_number, int):
            continue
        if not isinstance(milestone_title, str):
            continue
        if not isinstance(number, int):
            continue
        if not isinstance(title, str):
            continue

        rows.append(
            IssueRow(
                number=number,
                title=normalize_space(title),
                milestone_number=milestone_number,
                milestone_title=normalize_space(milestone_title),
                lane_labels=parse_lane_labels(raw.get("labels")),
            )
        )

    if not rows:
        raise ValueError("no open issue rows with milestones found in issues snapshot")
    return rows
