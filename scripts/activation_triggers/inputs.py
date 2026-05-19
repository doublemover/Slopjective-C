from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from activation_triggers.model import DEFAULT_ACTIONABLE_STATUSES


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"error: unable to read {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"error: invalid JSON in {display_path(path)}: {exc.msg} at {exc.lineno}:{exc.colno}"
        ) from exc


def count_items(root: Any, *, path: Path, label: str) -> int:
    if isinstance(root, list):
        return len(root)
    if isinstance(root, dict):
        items = root.get("items")
        if not isinstance(items, list):
            raise ValueError(
                f"error: {label} snapshot {display_path(path)} must be a JSON array or object with list field 'items'"
            )
        declared_count = root.get("count")
        if declared_count is not None:
            if isinstance(declared_count, bool) or not isinstance(declared_count, int):
                raise ValueError(
                    f"error: {label} snapshot field 'count' must be an integer in {display_path(path)}"
                )
            if declared_count != len(items):
                raise ValueError(
                    f"error: {label} snapshot field 'count'={declared_count} does not match item count {len(items)} in {display_path(path)}"
                )
        return len(items)
    raise ValueError(
        f"error: {label} snapshot {display_path(path)} must be a JSON array or object with list field 'items'"
    )


def count_actionable_catalog_rows(
    root: Any, *, path: Path, actionable_statuses: Sequence[str]
) -> int:
    if not isinstance(root, dict):
        raise ValueError(f"error: catalog snapshot {display_path(path)} must be a JSON object")
    tasks = root.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"error: catalog snapshot {display_path(path)} missing list field 'tasks'")
    actionable = set(actionable_statuses)
    count = 0
    for index, task in enumerate(tasks):
        if not isinstance(task, dict):
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} must be an object"
            )
        status = task.get("execution_status")
        if not isinstance(status, str) or not status.strip():
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} missing non-empty 'execution_status'"
            )
        if status in actionable:
            count += 1
    return count


def canonical_blocker_key(row: dict[str, Any], *, path: Path, index: int) -> tuple[str, str, int]:
    blocker_id = row.get("blocker_id")
    if not isinstance(blocker_id, str) or not blocker_id:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} missing non-empty 'blocker_id'"
        )
    source_path = row.get("source_path")
    if not isinstance(source_path, str) or not source_path or source_path.strip() != source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has invalid 'source_path'"
        )
    if source_path.startswith("/") or ":/" in source_path or ":\\" in source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} must use repository-relative 'source_path'"
        )
    if "\\" in source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} must use forward slashes in 'source_path'"
        )
    line_number = row.get("line_number")
    line_alias = row.get("line")
    if line_number is not None and (isinstance(line_number, bool) or not isinstance(line_number, int)):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has non-integer 'line_number'"
        )
    if line_alias is not None and (isinstance(line_alias, bool) or not isinstance(line_alias, int)):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has non-integer 'line'"
        )
    if line_number is not None and line_alias is not None and line_number != line_alias:
        raise ValueError(
            "error: open blockers snapshot line alias mismatch in "
            f"{display_path(path)} for row index {index}: line_number={line_number} line={line_alias}"
        )
    canonical_line = line_number if line_number is not None else line_alias
    if canonical_line is None:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} missing line metadata"
        )
    return (blocker_id, source_path, canonical_line)


def parse_open_blockers(root: Any, *, path: Path) -> tuple[int, list[tuple[str, str, int]]]:
    if not isinstance(root, dict):
        raise ValueError(
            f"error: open blockers snapshot {display_path(path)} must be a JSON object"
        )
    generated_at_utc = root.get("generated_at_utc")
    source = root.get("source")
    if (generated_at_utc is None) != (source is None):
        raise ValueError(
            "error: open blockers snapshot object must include both 'generated_at_utc' and 'source' "
            f"when either field is present in {display_path(path)}"
        )
    if generated_at_utc is not None and (not isinstance(generated_at_utc, str) or "T" not in generated_at_utc):
        raise ValueError(
            f"error: open blockers snapshot field 'generated_at_utc' must be an ISO-8601 string in {display_path(path)}"
        )
    if source is not None and (not isinstance(source, str) or not source or source.strip() != source):
        raise ValueError(
            f"error: open blockers snapshot field 'source' must be a non-empty trimmed string in {display_path(path)}"
        )

    rows = root.get("open_blockers")
    if not isinstance(rows, list):
        raise ValueError(
            f"error: open blockers snapshot object {display_path(path)} missing list field 'open_blockers'"
        )
    declared_count = root.get("open_blocker_count")
    if declared_count is not None and (isinstance(declared_count, bool) or not isinstance(declared_count, int) or declared_count < 0):
        raise ValueError(
            f"error: open blockers snapshot field 'open_blocker_count' must be a non-negative integer in {display_path(path)}"
        )

    canonical_rows: list[tuple[str, str, int]] = []
    seen: set[tuple[str, str, int]] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"error: open blockers snapshot row {index} in {display_path(path)} must be an object"
            )
        key = canonical_blocker_key(row, path=path, index=index)
        if key in seen:
            raise ValueError(
                "error: open blockers snapshot field 'open_blockers' contains duplicate canonical blocker row "
                f"{key!r} in {display_path(path)}"
            )
        seen.add(key)
        canonical_rows.append(key)

    expected_order = sorted(canonical_rows, key=lambda item: (item[1], item[2], item[0]))
    if canonical_rows != expected_order:
        raise ValueError(
            "error: open blockers snapshot field 'open_blockers' canonical rows must be sorted by "
            f"'source_path', then line number, then 'blocker_id' in {display_path(path)}"
        )
    if declared_count is not None and declared_count != len(rows):
        raise ValueError(
            "error: open blockers snapshot declared count does not match row count in "
            f"{display_path(path)}: declared={declared_count} actual={len(rows)}"
        )
    return len(rows), canonical_rows


def parse_t4_overlay(root: Any, *, path: Path | None, cli_flag: bool) -> tuple[bool, str]:
    if cli_flag:
        return True, "cli-flag"
    if path is None:
        return False, "default-false"
    if not isinstance(root, dict):
        raise ValueError(f"error: T4 governance overlay {display_path(path)} must be a JSON object")
    allowed_keys = {"t4_new_scope_publish"}
    unknown_keys = sorted(set(root.keys()) - allowed_keys)
    if unknown_keys:
        raise ValueError(
            f"error: T4 governance overlay {display_path(path)} contains unknown field(s): {', '.join(unknown_keys)}"
        )
    raw = root.get("t4_new_scope_publish")
    if not isinstance(raw, bool):
        raise ValueError(
            f"error: T4 governance overlay {display_path(path)} missing boolean field 't4_new_scope_publish'"
        )
    return raw, display_path(path)


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES
    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        value = raw.strip().lower()
        if not value:
            raise ValueError("error: actionable statuses must be non-empty strings")
        if value in seen:
            continue
        seen.add(value)
        normalized.append(value)
    if not normalized:
        raise ValueError("error: actionable statuses must be non-empty strings")
    return tuple(normalized)
