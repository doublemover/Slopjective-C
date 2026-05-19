from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from execution_microtasks.model import Issue
from execution_microtasks.normalization import normalize_inline_text


def parse_issue_number(raw: Any, index: int) -> int:
    if isinstance(raw, int):
        return raw

    if isinstance(raw, str) and raw.isdigit():
        return int(raw)

    raise ValueError(
        f"issue at index {index} has invalid 'number' value {raw!r}; expected int"
    )


def parse_issue_title(raw: Any, index: int) -> str:
    if not isinstance(raw, str):
        raise ValueError(
            f"issue at index {index} has invalid 'title' value {raw!r}; expected str"
        )
    return normalize_inline_text(raw)


def parse_labels(raw: Any) -> tuple[str, ...]:
    if raw is None:
        return ()
    if not isinstance(raw, list):
        return ()

    names: set[str] = set()
    for item in raw:
        if isinstance(item, str):
            label_name = normalize_inline_text(item)
            if label_name:
                names.add(label_name)
            continue
        if isinstance(item, dict):
            candidate = item.get("name")
            if isinstance(candidate, str):
                label_name = normalize_inline_text(candidate)
                if label_name:
                    names.add(label_name)
    return tuple(sorted(names, key=lambda label: (label.casefold(), label)))


def parse_catalog_task_identifier(raw: Any, index: int) -> str:
    if isinstance(raw, str):
        task_id = normalize_inline_text(raw)
        if task_id:
            return task_id
    return f"index:{index}"


def validate_catalog_status_integrity(path: Path, *, allow_missing_status: bool) -> None:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(
            f"catalog JSON file does not exist: {display_path(path)}"
        ) from exc
    except OSError as exc:
        raise ValueError(
            f"unable to read catalog JSON {display_path(path)}: {exc}"
        ) from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"invalid JSON in {display_path(path)}: {exc}"
        ) from exc

    if not isinstance(payload, dict):
        raise ValueError("catalog JSON root must be an object")

    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise ValueError("catalog JSON missing 'tasks' array")

    missing_status_task_ids: list[str] = []
    for index, raw_task in enumerate(raw_tasks):
        if not isinstance(raw_task, dict):
            raise ValueError(
                f"task at index {index} must be an object"
            )

        raw_status = raw_task.get("execution_status")
        if isinstance(raw_status, str) and raw_status.strip():
            continue

        missing_status_task_ids.append(
            parse_catalog_task_identifier(raw_task.get("task_id"), index)
        )

    if not missing_status_task_ids or allow_missing_status:
        return

    preview = ", ".join(missing_status_task_ids[:5])
    if len(missing_status_task_ids) > 5:
        preview += ", ..."

    raise ValueError(
        "catalog status integrity check failed: "
        f"{len(missing_status_task_ids)} task(s) are missing required "
        f"'execution_status' (examples: {preview}); pass "
        "--allow-missing-status to continue"
    )


def load_issues(path: Path) -> list[Issue]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"issues JSON file does not exist: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(payload, list):
        raise ValueError("issues JSON root must be an array")

    issues: list[Issue] = []
    for index, raw_issue in enumerate(payload):
        if not isinstance(raw_issue, dict):
            raise ValueError(
                f"issue at index {index} must be an object, got {type(raw_issue).__name__}"
            )

        number = parse_issue_number(raw_issue.get("number"), index)
        title = parse_issue_title(raw_issue.get("title"), index)
        labels = parse_labels(raw_issue.get("labels"))
        issues.append(Issue(number=number, title=title, labels=labels))

    return sorted(issues, key=lambda issue: (issue.number, issue.title.casefold(), issue.title))
