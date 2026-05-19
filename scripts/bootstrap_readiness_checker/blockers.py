from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def canonical_blocker_key(
    row: dict[str, Any],
    *,
    path: Path,
    index: int,
) -> tuple[str, str, int]:
    blocker_id = row.get("blocker_id")
    if not isinstance(blocker_id, str) or not blocker_id:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} "
            "missing non-empty 'blocker_id'"
        )
    source_path = row.get("source_path", "")
    if source_path is None:
        source_path = ""
    if not isinstance(source_path, str):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} "
            "has non-string 'source_path'"
        )
    line_number = row.get("line_number")
    line_alias = row.get("line")
    if line_number is not None and (
        isinstance(line_number, bool) or not isinstance(line_number, int)
    ):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} "
            "has non-integer 'line_number'"
        )
    if line_alias is not None and (
        isinstance(line_alias, bool) or not isinstance(line_alias, int)
    ):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} "
            "has non-integer 'line'"
        )
    if line_number is not None and line_alias is not None and line_number != line_alias:
        raise ValueError(
            "error: open blockers snapshot line alias mismatch in "
            f"{display_path(path)} for row index {index}: "
            f"line_number={line_number} line={line_alias}"
        )
    canonical_line = line_number if line_number is not None else line_alias
    if canonical_line is None:
        canonical_line = 0
    return (blocker_id, source_path, canonical_line)


def extract_open_blocker_rows(root: Any, *, path: Path) -> tuple[list[Any], int | None]:
    declared_count: int | None = None
    if isinstance(root, list):
        return root, declared_count
    if not isinstance(root, dict):
        raise ValueError(
            f"error: open blockers snapshot {display_path(path)} must be a JSON array or object"
        )

    generated_at_utc = root.get("generated_at_utc")
    source = root.get("source")
    if (generated_at_utc is None) != (source is None):
        raise ValueError(
            "error: open blockers snapshot object must include both "
            "'generated_at_utc' and 'source' when either field is present in "
            f"{display_path(path)}"
        )
    rows = root.get("open_blockers")
    if not isinstance(rows, list):
        raise ValueError(
            f"error: open blockers snapshot object {display_path(path)} "
            "missing list field 'open_blockers'"
        )
    open_blocker_count = root.get("open_blocker_count")
    count = root.get("count")
    for label, raw in (("open_blocker_count", open_blocker_count), ("count", count)):
        if raw is not None and (
            isinstance(raw, bool) or not isinstance(raw, int) or raw < 0
        ):
            raise ValueError(
                f"error: open blockers snapshot field '{label}' must be a "
                f"non-negative integer in {display_path(path)}"
            )
    if open_blocker_count is not None and count is not None and open_blocker_count != count:
        raise ValueError(
            "error: open blockers snapshot metadata count mismatch in "
            f"{display_path(path)}: open_blocker_count={open_blocker_count} count={count}"
        )
    declared_count = open_blocker_count if open_blocker_count is not None else count
    return rows, declared_count


def count_open_blockers(root: Any, *, path: Path) -> int:
    rows, declared_count = extract_open_blocker_rows(root, path=path)

    canonical_rows: list[tuple[str, str, int]] = []
    seen: set[tuple[str, str, int]] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"error: open blockers snapshot row {index} in {display_path(path)} "
                "must be an object"
            )
        key = canonical_blocker_key(row, path=path, index=index)
        if key in seen:
            raise ValueError(
                "error: open blockers snapshot field 'open_blockers' contains "
                f"duplicate canonical blocker row {key!r} in {display_path(path)}"
            )
        seen.add(key)
        canonical_rows.append(key)

    expected_order = sorted(canonical_rows, key=lambda item: (item[1], item[2], item[0]))
    if canonical_rows != expected_order:
        raise ValueError(
            "error: open blockers snapshot field 'open_blockers' canonical rows must "
            "be sorted by 'source_path', then line number, then 'blocker_id' in "
            f"{display_path(path)}"
        )

    if declared_count is not None and declared_count != len(rows):
        raise ValueError(
            "error: open blockers snapshot declared count does not match row count in "
            f"{display_path(path)}: declared={declared_count} actual={len(rows)}"
        )
    return len(rows)
