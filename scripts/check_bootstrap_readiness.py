#!/usr/bin/env python3
"""Check deterministic bootstrap readiness for zero-open-state gating."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
ACTIONABLE_CATALOG_STATUSES = frozenset({"open", "open-blocked", "blocked"})


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def load_json(path: Path, *, label: str) -> Any:
    if not path.exists():
        raise ValueError(f"{label} JSON file does not exist: {display_path(path)}")
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"unable to read {label} JSON {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {label} file {display_path(path)}: {exc}") from exc


def parse_non_negative_int(raw_value: Any, *, context: str) -> int:
    if isinstance(raw_value, bool) or not isinstance(raw_value, int) or raw_value < 0:
        raise ValueError(f"{context} must be a non-negative integer")
    return raw_value


def parse_positive_int(raw_value: Any, *, context: str) -> int:
    if isinstance(raw_value, bool) or not isinstance(raw_value, int) or raw_value <= 0:
        raise ValueError(f"{context} must be a positive integer")
    return raw_value


def parse_canonical_non_empty_string(raw_value: Any, *, context: str) -> str:
    if not isinstance(raw_value, str):
        raise ValueError(f"{context} must be a non-empty string")
    if not raw_value.strip():
        raise ValueError(f"{context} must be a non-empty string")
    if raw_value != raw_value.strip():
        raise ValueError(f"{context} must not contain leading/trailing whitespace")
    return raw_value


def parse_canonical_relative_posix_path(raw_value: Any, *, context: str) -> str:
    value = parse_canonical_non_empty_string(raw_value, context=context)
    if "\\" in value:
        raise ValueError(f"{context} must use '/' path separators")
    if value.startswith("/"):
        raise ValueError(f"{context} must be a relative path")
    if len(value) >= 3 and value[0].isalpha() and value[1] == ":" and value[2] == "/":
        raise ValueError(f"{context} must be a relative path")

    segments = value.split("/")
    if any(segment == "" for segment in segments):
        raise ValueError(
            f"{context} must be normalized (no empty path segments or trailing slash)"
        )
    if any(segment in {".", ".."} for segment in segments):
        raise ValueError(
            f"{context} must be normalized ('.' and '..' segments are not allowed)"
        )
    return value


def parse_generated_at_utc(raw_value: Any, *, context: str) -> datetime:
    if not isinstance(raw_value, str) or not raw_value.strip():
        raise ValueError(f"{context} must be a non-empty ISO-8601 timestamp")

    normalized = raw_value.strip()
    if normalized.endswith("Z"):
        normalized = f"{normalized[:-1]}+00:00"

    try:
        parsed = datetime.fromisoformat(normalized)
    except ValueError as exc:
        raise ValueError(f"{context} must be a valid ISO-8601 timestamp") from exc

    if parsed.tzinfo is None:
        raise ValueError(f"{context} must include a timezone (for example 'Z')")

    return parsed.astimezone(timezone.utc)


def normalize_status(raw_value: Any) -> str:
    if not isinstance(raw_value, str):
        return ""
    return raw_value.strip().lower()


def count_snapshot_rows(payload: Any, *, label: str, array_keys: Sequence[str]) -> int:
    if isinstance(payload, list):
        return len(payload)

    if not isinstance(payload, dict):
        raise ValueError(f"{label} snapshot must be either an array or object")

    lengths: dict[str, int] = {}
    for key in array_keys:
        if key not in payload:
            continue
        raw_rows = payload[key]
        if not isinstance(raw_rows, list):
            raise ValueError(f"{label} snapshot field '{key}' must be an array")
        lengths[key] = len(raw_rows)

    discovered_count: int | None = None
    if lengths:
        first_key = next(iter(lengths))
        discovered_count = lengths[first_key]
        for key, count in lengths.items():
            if count != discovered_count:
                raise ValueError(
                    f"{label} snapshot list-length mismatch: '{first_key}'={discovered_count} "
                    f"but '{key}'={count}"
                )

    declared_count: int | None = None
    if "count" in payload:
        declared_count = parse_non_negative_int(
            payload["count"],
            context=f"{label} snapshot field 'count'",
        )

    if discovered_count is not None and declared_count is not None and discovered_count != declared_count:
        raise ValueError(
            f"{label} snapshot count mismatch: discovered {discovered_count} != declared {declared_count}"
        )
    if discovered_count is not None:
        return discovered_count
    if declared_count is not None:
        return declared_count

    joined_keys = ", ".join(f"'{key}'" for key in array_keys)
    raise ValueError(
        f"{label} snapshot object must include at least one of {joined_keys} arrays or a 'count' field"
    )


def count_catalog_open_tasks(payload: Any) -> int:
    tasks: Any
    if isinstance(payload, list):
        tasks = payload
    elif isinstance(payload, dict):
        if "tasks" in payload:
            tasks = payload["tasks"]
        elif "items" in payload:
            tasks = payload["items"]
        else:
            raise ValueError("remaining-task catalog must include a 'tasks' array (or compatible 'items')")
    else:
        raise ValueError("remaining-task catalog must be an array or object")

    if not isinstance(tasks, list):
        raise ValueError("remaining-task catalog tasks payload must be an array")

    open_task_count = 0
    for index, row in enumerate(tasks):
        if not isinstance(row, dict):
            raise ValueError(f"catalog task at index {index} must be an object")
        status = normalize_status(row.get("execution_status", row.get("status")))
        if status in ACTIONABLE_CATALOG_STATUSES:
            open_task_count += 1

    return open_task_count


def validate_optional_canonical_blocker_rows(
    rows: Sequence[Any],
    *,
    row_collection_name: str,
    snapshot_path: Path | None = None,
) -> None:
    snapshot_path_text = (
        f" in {display_path(snapshot_path)}" if snapshot_path is not None else ""
    )
    canonical_sort_keys: list[tuple[str, str, int]] = []
    seen_canonical_rows: set[tuple[str, str, int]] = set()
    partial_canonical_rows: list[int] = []

    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"open blockers snapshot field '{row_collection_name}[{index}]' must be an object"
                f"{snapshot_path_text}"
            )

        has_blocker_id = "blocker_id" in row
        has_source_path = "source_path" in row
        has_line_number = "line_number" in row
        has_line_alias = "line" in row
        has_any_canonical_field = has_blocker_id or has_source_path or has_line_number or has_line_alias
        has_full_canonical_fields = has_blocker_id and has_source_path and (has_line_number or has_line_alias)
        if not has_any_canonical_field:
            continue
        if not has_full_canonical_fields:
            partial_canonical_rows.append(index)
            continue

        blocker_id = parse_canonical_non_empty_string(
            row.get("blocker_id"),
            context=f"open blockers snapshot field '{row_collection_name}[{index}].blocker_id'",
        )
        source_path = parse_canonical_relative_posix_path(
            row.get("source_path"),
            context=f"open blockers snapshot field '{row_collection_name}[{index}].source_path'",
        )
        if has_line_number and has_line_alias:
            canonical_line_number = parse_positive_int(
                row.get("line_number"),
                context=f"open blockers snapshot field '{row_collection_name}[{index}].line_number'",
            )
            legacy_line_number = parse_positive_int(
                row.get("line"),
                context=f"open blockers snapshot field '{row_collection_name}[{index}].line'",
            )
            if canonical_line_number != legacy_line_number:
                raise ValueError(
                    "open blockers snapshot line alias mismatch in "
                    f"{display_path(snapshot_path) if snapshot_path is not None else 'inline payload'} "
                    f"for row index {index}: line_number={canonical_line_number} line={legacy_line_number}"
                )
            line_number = canonical_line_number
        else:
            line_number = parse_positive_int(
                row.get("line_number", row.get("line")),
                context=(
                    f"open blockers snapshot field '{row_collection_name}[{index}].line_number' "
                    "(or legacy 'line')"
                ),
            )

        canonical_row_key = (blocker_id, source_path, line_number)
        if canonical_row_key in seen_canonical_rows:
            raise ValueError(
                "open blockers snapshot field "
                f"'{row_collection_name}' contains duplicate canonical blocker row {canonical_row_key!r}"
                f"{snapshot_path_text}"
            )
        seen_canonical_rows.add(canonical_row_key)
        canonical_sort_keys.append(canonical_row_key)

    if canonical_sort_keys and partial_canonical_rows:
        partial_row_indexes_text = ", ".join(str(index) for index in partial_canonical_rows)
        raise ValueError(
            "open blockers snapshot field "
            f"'{row_collection_name}' mixes canonical rows with partial canonical rows at index(es) "
            f"[{partial_row_indexes_text}]; canonical rows require "
            "'blocker_id', 'source_path', and 'line_number' (or legacy 'line')"
            f"{snapshot_path_text}"
        )

    if not canonical_sort_keys:
        return

    expected_order = sorted(
        canonical_sort_keys,
        key=lambda row: (
            row[1].casefold(),
            row[1],
            row[2],
            row[0].casefold(),
            row[0],
        ),
    )
    if canonical_sort_keys != expected_order:
        raise ValueError(
            "open blockers snapshot field "
            f"'{row_collection_name}' canonical rows must be sorted by "
            "'source_path', then line number, then 'blocker_id'"
            f"{snapshot_path_text}"
        )


def count_open_blockers(payload: Any, *, snapshot_path: Path | None = None) -> int:
    snapshot_path_text = (
        f" in {display_path(snapshot_path)}" if snapshot_path is not None else ""
    )

    # Preserve compatibility for legacy extractor payloads that emit a bare array of rows.
    if isinstance(payload, list):
        return len(payload)

    if not isinstance(payload, dict):
        raise ValueError(
            "open blockers payload must be an array or object"
            f"{snapshot_path_text}"
        )

    has_generated_at_utc = "generated_at_utc" in payload
    has_source = "source" in payload
    if has_generated_at_utc != has_source:
        raise ValueError(
            "open blockers snapshot object must include both 'generated_at_utc' and "
            f"'source' when either field is present{snapshot_path_text}"
        )
    if has_generated_at_utc:
        try:
            parse_generated_at_utc(
                payload["generated_at_utc"],
                context="open blockers snapshot field 'generated_at_utc'",
            )
        except ValueError as exc:
            raise ValueError(f"{exc}{snapshot_path_text}") from exc
        try:
            parse_canonical_non_empty_string(
                payload["source"],
                context="open blockers snapshot field 'source'",
            )
        except ValueError as exc:
            raise ValueError(f"{exc}{snapshot_path_text}") from exc

    list_keys = ("open_blockers", "blockers", "items", "list", "open")
    lengths: dict[str, int] = {}
    for key in list_keys:
        if key not in payload:
            continue
        raw_rows = payload[key]
        if not isinstance(raw_rows, list):
            raise ValueError(
                f"open blockers field '{key}' must be an array{snapshot_path_text}"
            )
        validate_optional_canonical_blocker_rows(
            raw_rows,
            row_collection_name=key,
            snapshot_path=snapshot_path,
        )
        lengths[key] = len(raw_rows)

    discovered_count: int | None = None
    if lengths:
        first_key = next(iter(lengths))
        discovered_count = lengths[first_key]
        for key, count in lengths.items():
            if count != discovered_count:
                raise ValueError(
                    "open blockers list-length mismatch: "
                    f"'{first_key}'={discovered_count} but '{key}'={count}{snapshot_path_text}"
                )

    declared_count: int | None = None
    if "open_blocker_count" in payload:
        declared_count = parse_non_negative_int(
            payload["open_blocker_count"],
            context="open blockers field 'open_blocker_count'",
        )
    if "count" in payload:
        count_value = parse_non_negative_int(
            payload["count"],
            context="open blockers field 'count'",
        )
        if declared_count is None:
            declared_count = count_value
        elif declared_count != count_value:
            raise ValueError(
                "open blockers fields 'open_blocker_count' and 'count' must match when both are present"
                f"{snapshot_path_text}"
            )

    if discovered_count is not None and declared_count is not None and discovered_count != declared_count:
        raise ValueError(
            "open blockers count mismatch: "
            f"discovered {discovered_count} != declared {declared_count}{snapshot_path_text}"
        )
    if discovered_count is not None:
        return discovered_count
    if declared_count is not None:
        return declared_count

    raise ValueError(
        "open blockers object must include a list field or count field"
        f"{snapshot_path_text}"
    )


def build_payload(
    *,
    issues_open_count: int,
    milestones_open_count: int,
    catalog_open_task_count: int,
    blockers_open_count: int,
) -> dict[str, Any]:
    dimension_pairs = (
        ("issues_open_count", issues_open_count),
        ("milestones_open_count", milestones_open_count),
        ("catalog_open_task_count", catalog_open_task_count),
        ("blockers_open_count", blockers_open_count),
    )
    blocking_dimensions = [name for name, count in dimension_pairs if count > 0]
    bootstrappable = not blocking_dimensions

    return {
        "issues_open_count": issues_open_count,
        "milestones_open_count": milestones_open_count,
        "catalog_open_task_count": catalog_open_task_count,
        "blockers_open_count": blockers_open_count,
        "readiness_state": "bootstrappable" if bootstrappable else "blocked",
        "intake_recommendation": "go" if bootstrappable else "hold",
        "blocking_dimensions": blocking_dimensions,
    }


def render_json(payload: dict[str, Any]) -> str:
    return json.dumps(payload, indent=2) + "\n"


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
        description="Determine bootstrap readiness from open-state snapshots.",
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to open-issues JSON (array or compatible object).",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to open-milestones JSON (array or compatible object).",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        required=True,
        help="Path to remaining_task_review_catalog.json (or compatible shape).",
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help="Optional path to open blockers JSON (array or object with count/list).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "md"),
        default="json",
        help="Output format: json (default) or md.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        issues_path = resolve_input_path(args.issues_json)
        milestones_path = resolve_input_path(args.milestones_json)
        catalog_path = resolve_input_path(args.catalog_json)
        open_blockers_path = (
            resolve_input_path(args.open_blockers_json)
            if args.open_blockers_json is not None
            else None
        )

        issues_payload = load_json(issues_path, label="issues")
        milestones_payload = load_json(milestones_path, label="milestones")
        catalog_payload = load_json(catalog_path, label="catalog")

        payload = build_payload(
            issues_open_count=count_snapshot_rows(
                issues_payload,
                label="issues",
                array_keys=("items", "open", "issues"),
            ),
            milestones_open_count=count_snapshot_rows(
                milestones_payload,
                label="milestones",
                array_keys=("items", "open", "milestones"),
            ),
            catalog_open_task_count=count_catalog_open_tasks(catalog_payload),
            blockers_open_count=(
                0
                if open_blockers_path is None
                else count_open_blockers(
                    load_json(open_blockers_path, label="open blockers"),
                    snapshot_path=open_blockers_path,
                )
            ),
        )
    except (ValueError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    except Exception as exc:  # pragma: no cover - explicit runtime guard for exit semantics.
        print(f"error: unexpected runtime failure: {exc}", file=sys.stderr)
        return 2

    if args.format == "md":
        write_stdout(render_markdown(payload))
    else:
        write_stdout(render_json(payload))

    return 0 if payload["readiness_state"] == "bootstrappable" else 1


if __name__ == "__main__":
    raise SystemExit(main())
