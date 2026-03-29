#!/usr/bin/env python3
"""Compute deterministic bootstrap readiness from offline snapshots."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]

EXIT_BOOTSTRAPPABLE = 0
EXIT_BLOCKED = 1
EXIT_HARD_FAILURE = 2


def normalize_newlines(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_repo_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


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


def count_open_catalog_tasks(root: Any, *, path: Path) -> int:
    if not isinstance(root, dict):
        raise ValueError(f"error: catalog snapshot {display_path(path)} must be a JSON object")
    tasks = root.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"error: catalog snapshot {display_path(path)} missing list field 'tasks'")

    open_count = 0
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
        if status not in {"closed", "done"}:
            open_count += 1
    return open_count


def canonical_blocker_key(row: dict[str, Any], *, path: Path, index: int) -> tuple[str, str, int]:
    blocker_id = row.get("blocker_id")
    if not isinstance(blocker_id, str) or not blocker_id:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} missing non-empty 'blocker_id'"
        )
    source_path = row.get("source_path", "")
    if source_path is None:
        source_path = ""
    if not isinstance(source_path, str):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has non-string 'source_path'"
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
        canonical_line = 0
    return (blocker_id, source_path, canonical_line)


def count_open_blockers(root: Any, *, path: Path) -> int:
    rows: list[Any]
    declared_count: int | None = None
    if isinstance(root, list):
        rows = root
    elif isinstance(root, dict):
        generated_at_utc = root.get("generated_at_utc")
        source = root.get("source")
        if (generated_at_utc is None) != (source is None):
            raise ValueError(
                "error: open blockers snapshot object must include both 'generated_at_utc' and 'source' "
                f"when either field is present in {display_path(path)}"
            )
        rows = root.get("open_blockers")
        if not isinstance(rows, list):
            raise ValueError(
                f"error: open blockers snapshot object {display_path(path)} missing list field 'open_blockers'"
            )
        open_blocker_count = root.get("open_blocker_count")
        count = root.get("count")
        for label, raw in (("open_blocker_count", open_blocker_count), ("count", count)):
            if raw is not None and (isinstance(raw, bool) or not isinstance(raw, int) or raw < 0):
                raise ValueError(
                    f"error: open blockers snapshot field '{label}' must be a non-negative integer in {display_path(path)}"
                )
        if open_blocker_count is not None and count is not None and open_blocker_count != count:
            raise ValueError(
                "error: open blockers snapshot metadata count mismatch in "
                f"{display_path(path)}: open_blocker_count={open_blocker_count} count={count}"
            )
        declared_count = open_blocker_count if open_blocker_count is not None else count
    else:
        raise ValueError(
            f"error: open blockers snapshot {display_path(path)} must be a JSON array or object"
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
    return len(rows)


def build_payload(
    *,
    issues_open_count: int,
    milestones_open_count: int,
    catalog_open_task_count: int,
    blockers_open_count: int,
) -> dict[str, Any]:
    blocking_dimensions = [
        field_name
        for field_name, count in (
            ("issues_open_count", issues_open_count),
            ("milestones_open_count", milestones_open_count),
            ("catalog_open_task_count", catalog_open_task_count),
            ("blockers_open_count", blockers_open_count),
        )
        if count > 0
    ]
    readiness_state = "bootstrappable" if not blocking_dimensions else "blocked"
    intake_recommendation = "go" if readiness_state == "bootstrappable" else "hold"
    return {
        "issues_open_count": issues_open_count,
        "milestones_open_count": milestones_open_count,
        "catalog_open_task_count": catalog_open_task_count,
        "blockers_open_count": blockers_open_count,
        "readiness_state": readiness_state,
        "intake_recommendation": intake_recommendation,
        "blocking_dimensions": blocking_dimensions,
    }


def render_markdown(payload: dict[str, Any]) -> str:
    lines = [
        "# Bootstrap Readiness",
        "",
        "| Metric | Value |",
        "| --- | --- |",
        f"| issues_open_count | `{payload['issues_open_count']}` |",
        f"| milestones_open_count | `{payload['milestones_open_count']}` |",
        f"| catalog_open_task_count | `{payload['catalog_open_task_count']}` |",
        f"| blockers_open_count | `{payload['blockers_open_count']}` |",
        f"| readiness_state | `{payload['readiness_state']}` |",
        f"| intake_recommendation | `{payload['intake_recommendation']}` |",
    ]
    return "\n".join(lines) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_bootstrap_readiness.py",
        description="Compute deterministic bootstrap readiness from offline issue, milestone, catalog, and blocker snapshots.",
    )
    parser.add_argument("--issues-json", type=Path, required=True)
    parser.add_argument("--milestones-json", type=Path, required=True)
    parser.add_argument("--catalog-json", type=Path, required=True)
    parser.add_argument("--open-blockers-json", type=Path)
    parser.add_argument("--format", choices=("json", "md"), default="json")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json) if args.open_blockers_json is not None else None
    )

    try:
        issues_open_count = count_items(load_json(issues_path), path=issues_path, label="issues")
        milestones_open_count = count_items(
            load_json(milestones_path), path=milestones_path, label="milestones"
        )
        catalog_open_task_count = count_open_catalog_tasks(load_json(catalog_path), path=catalog_path)
        blockers_open_count = 0
        if open_blockers_path is not None:
            blockers_open_count = count_open_blockers(
                load_json(open_blockers_path), path=open_blockers_path
            )
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return EXIT_HARD_FAILURE

    payload = build_payload(
        issues_open_count=issues_open_count,
        milestones_open_count=milestones_open_count,
        catalog_open_task_count=catalog_open_task_count,
        blockers_open_count=blockers_open_count,
    )
    output = (
        json.dumps(payload, indent=2) + "\n"
        if args.format == "json"
        else render_markdown(payload)
    )
    sys.stdout.write(normalize_newlines(output))
    return EXIT_BOOTSTRAPPABLE if payload["readiness_state"] == "bootstrappable" else EXIT_BLOCKED


if __name__ == "__main__":
    raise SystemExit(main())
